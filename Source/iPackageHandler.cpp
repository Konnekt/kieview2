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

string iPackageHandler::getRepoPath(const string& path) {
  return getFileDirectory(path) + "\\" + _repoDir;
}

void iPackageHandler::prepareRepo(const string& path) {
  string localPath = getRepoPath(path);

  if (!isDirectory(localPath.c_str())) {
    try {
      Zip zip(path);
      zip.unzipDir(localPath, zip.get(0).getDirectory());
      zip.close();
    } catch(const Exception& e) {
      throw CannotOpen(e.getReason());
    }
    SetFileAttributes(localPath.c_str(), GetFileAttributes(localPath.c_str()) | FILE_ATTRIBUTE_HIDDEN);
  }
}

string iPackageHandler::getKonnektPath() {
  return Controller::getInstance()->kPath;
}

string iPackageHandler::getDir() {
  if (!_dirColID) return "";

  string dir = Controller::getConfig()->getChar(_dirColID);
  dir = (dir.find(':') == dir.npos) ? getKonnektPath() + dir : dir;
  return dir;
}

void iPackageHandler::loadPackages() {
  clearPackages();

  FindFile find;
  find.setMask(getDir() + "\\*");
  find.setDirOnly();

  FindFile::tFoundFiles dirs;
  dirs = find.makeList();

  if (find.nothingFound()) {
    IMLOG("[iPackageHandler::loadPackages()] Nie znaleziono katalogu z pakietami !");
    return;
  }
  if (!dirs.size()) {
    IMLOG("[iPackageHandler::loadPackages()] Brak katalog�w z pakietami !");
    return;
  }

  for (tParsers::iterator parser = _parsers.begin(); parser != _parsers.end(); parser++) {
    for (FindFile::tFoundFiles::iterator dir = dirs.begin(); dir != dirs.end(); dir++) {
      string defPath = dir->getFilePath() + "\\";

      FindFileFiltered files(defPath + "*");
      files.setFileOnly();

      if ((*parser)->fromArchive()) {
        files.setMask(defPath + (*parser)->getArchiveMask());
        files.find();
        if (!files.nothingFound() && !files.found().empty()) {
          prepareRepo(files.found().getFilePath());
          defPath = getRepoPath(defPath) + "\\";
        }
      }

      files.setMask(defPath + (*parser)->getDefinitionMask());
      files.find();

      if (files.nothingFound() || files.found().empty()) {
        continue;
      }
      try {
        iPackage* package = (*parser)->parse(files.found());
        if (!package->getName().length()) {
          package->setName(dir->getFileName());
        }
        package->setDir(dir->getFileName());

        *this << package;
      } catch (const Exception& e) {
        IMLOG("[iPackageHandler::loadPackages()] b��d podczas parsowania paczki (%s): %s", 
          dir->getFileName().c_str(), e.getReason().c_str());
      }
    }
  }
}