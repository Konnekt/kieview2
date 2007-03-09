/**
  *  kIEview2 Package handler base class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 224 $
  *  @modifiedby   $LastChangedBy: ursus6 $
  *  @lastmodified $Date: 2007-02-19 20:44:26 +0100 (Pn, 19 lut 2007) $
  */

#include "stdafx.h"

#include "iPackageHandler.h"
#include "Controller.h"

string iPackageHandler::getRepoPath(const string& path, bool inPackageDir) {
  string repoPath = getFileDirectory(path) + "\\";

  if (inPackageDir) {
    return repoPath + "~local";
  }
  repoPath += getFileName(path);
  repoPath = repoPath.substr(0, repoPath.find_last_of('.')) + "\\";

  return repoPath;
}

void iPackageHandler::prepareRepo(FindFile::Found& package, iPackageParser* parser, bool inPackageDir) {
  string localPath = getRepoPath(package.getFilePath(), inPackageDir);

  HANDLE lockfile = CreateFile((localPath + "\\template.lock").c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
  HANDLE zipfile = CreateFile(package.getFilePath().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

  bool canDelete = false;

  if (lockfile != (HANDLE)-1 && zipfile != (HANDLE)-1) {
    FILETIME ctLock, ctZip;
    GetFileTime(zipfile, &ctZip, NULL, NULL);
    GetFileTime(lockfile, &ctLock, NULL, NULL);
    if (CompareFileTime(&ctZip, &ctLock)) {
      canDelete = true;
    }
  }

  CloseHandle(lockfile);
  CloseHandle(zipfile);

  if (canDelete) removeDirTree(localPath);

  if (!isDirectory(localPath.c_str())) {
    try {
      Zip zip(package.getFilePath());
      zip.unzipDir(localPath, zip.find(parser->getDefinitionMask()).getDirectory());
      zip.close();

      HANDLE lockfile = CreateFile((localPath + "\\template.lock").c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, NULL, NULL);
      HANDLE zipfile = CreateFile(package.getFilePath().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

      FILETIME ct;
      GetFileTime(zipfile, &ct, NULL, NULL);
      SetFileTime(lockfile, &ct, &ct, &ct);
      CloseHandle(lockfile);
      CloseHandle(zipfile);

    } catch(const Exception& e) {
      if (isDirectory(localPath.c_str())) {
        removeDirTree(localPath);
      }
      throw CannotOpen(e.getReason());
    }
    SetFileAttributes(localPath.c_str(), GetFileAttributes(localPath.c_str()) | FILE_ATTRIBUTE_HIDDEN);
  }
}

string iPackageHandler::getKonnektPath() {
  return Controller::getInstance()->kPath;
}

string iPackageHandler::getDir(tColId dirColID) {
  string dir = Controller::getConfig()->getChar(dirColID);
  dir = (dir.find(':') == dir.npos) ? getKonnektPath() + dir : dir;
  return dir;
}

iPackage* iPackageHandler::loadPackage(iPackageParser* parser, FindFile::Found& dir) {
  string defPath = dir.getFilePath() + "\\";

  FindFileFiltered files(defPath + "*");
  files.setFileOnly();

  if (parser->fromArchive()) {
    files.setMask(defPath + parser->getArchiveMask());

    if (files.find() && !files.found().empty()) {
      prepareRepo((FindFile::Found&) files.found(), parser, true);
      defPath = getRepoPath(defPath, true) + "\\";
    }
  }

  files.setMask(defPath + parser->getDefinitionMask());
  if (!files.find() || files.found().empty()) {
    throw kException(kException::typeMajor, "Brak pliku z definicj¹ paczki");
  }

  iPackage* package = parser->parse(files.found());
  package->setDir(files.found().getDirectory());

  if (!package->getName().length()) {
    package->setName(dir.getFileName());
  }
  if (!package->getID().length()) {
    package->setID(makeID(package->getName()));
  }
  return package;
}

void iPackageHandler::preparePackages(iPackageParser* parser, FindFile::Found& dir) {
  if (!parser->fromArchive()) return;

  FindFileFiltered ff(dir.getFilePath() + "\\" + parser->getArchiveMask());
  ff.setFileOnly();

  FindFile::tFoundFiles files;
  files = ff.makeList();

  if (!files.size()) {
    return;
  }
  for (FindFile::tFoundFiles::iterator file = files.begin(); file != files.end(); file++) {
    prepareRepo(*file, parser, false);
  }
}

void iPackageHandler::loadPackages(const string& dir) {
  clearPackages();

  for (tParsers::iterator parser = _parsers.begin(); parser != _parsers.end(); parser++) {
    if (!(*parser)->fromArchive()) continue;
    try {
      preparePackages(*parser, FindFile::Found(dir));
    } catch(...) { }
  }

  FindFile ff;
  ff.setMask(dir + "\\*");
  ff.setDirOnly();

  FindFile::tFoundFiles dirs;
  dirs = ff.makeList();

  if (ff.nothingFound() || !dirs.size()) {
    IMLOG("[iPackageHandler::loadPackages()] Brak katalogów z paczkami !");
    return;
  }

  for (FindFile::tFoundFiles::iterator dir = dirs.begin(); dir != dirs.end(); dir++) {
    for (tParsers::iterator parser = _parsers.begin(); parser != _parsers.end(); parser++) {
      try {
        *this << loadPackage(*parser, *dir);
        break;
      } catch (const ExceptionString& e) {
        if (e.hasReason()) {
          IMLOG("[iPackageHandler::loadPackages()] b³¹d podczas parsowania paczki (%s): %s", 
            dir->getFileName().c_str(), e.getReason().c_str());
        }
      }
    }
  }
}