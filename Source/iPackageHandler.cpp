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

  if (!inPackageDir) {
    repoPath += getFileName(path);
    repoPath = repoPath.substr(0, repoPath.find_last_of('.'));
  } else {
    repoPath += "~local";
  }
  return repoPath + "\\";
}

void iPackageHandler::prepareRepo(FindFile::Found& package, iPackageParser* parser, bool inPackageDir) {
  string repoPath = getRepoPath(package.getFilePath(), inPackageDir);
  string lockFile = repoPath + "package.lock";

  if (isDirectory(repoPath.c_str()) && fileExists(lockFile.c_str())) {
    bool expired = false;

    HANDLE lockfile = CreateFile(lockFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    HANDLE zipfile = CreateFile(package.getFilePath().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

    if (lockfile != INVALID_HANDLE_VALUE && zipfile != INVALID_HANDLE_VALUE) {
      FILETIME ctLock, ctZip;
      GetFileTime(lockfile, &ctLock, NULL, NULL);
      GetFileTime(zipfile, NULL, NULL, &ctZip);

      if (CompareFileTime(&ctZip, &ctLock) == 1) {
        expired = true;
      }
    }
    CloseHandle(lockfile);
    CloseHandle(zipfile);

    if (expired) {
      removeDirTree(repoPath);
    }
  }

  if (!isDirectory(repoPath.c_str())) {
    try {
      Zip zip(package.getFilePath());
      zip.unzipDir(repoPath, zip.find(parser->getDefinitionMask()).getDirectory());
      zip.close();

      HANDLE lockfile = CreateFile(lockFile.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, NULL, NULL);
      HANDLE zipfile = CreateFile(package.getFilePath().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

      FILETIME ct;
      GetFileTime(zipfile, NULL, NULL, &ct);
      SetFileTime(lockfile, &ct, &ct, &ct);

      CloseHandle(lockfile);
      CloseHandle(zipfile);

      SetFileAttributes(lockFile.c_str(), GetFileAttributes(lockFile.c_str()) | FILE_ATTRIBUTE_HIDDEN);
      SetFileAttributes(repoPath.c_str(), GetFileAttributes(repoPath.c_str()) | FILE_ATTRIBUTE_HIDDEN);

    } catch(const Exception& e) {
      if (isDirectory(repoPath.c_str())) {
        removeDirTree(repoPath);
      }
      throw CannotOpen(e.getReason());
    }
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
      defPath = getRepoPath(defPath, true);
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