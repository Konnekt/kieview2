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

void iPackageHandler::prepareRepo(const string& path, const string& rootdir) {
  string localPath = getFileDirectory(path) + "\\" + _repoDir;
  Zip zip;

  try {
    zip.open(path);
    zip.unzipDir(localPath, rootdir);
    zip.close();
  } catch(const Exception& e) {
    throw CannotOpen(e.getReason());
  }
  SetFileAttributes(localPath.c_str(), GetFileAttributes(localPath.c_str()) & FILE_ATTRIBUTE_HIDDEN);
}

string iPackageHandler::getKonnektPath() {
  return Controller::getInstance()->kPath;
}

string iPackageHandler::getDir() {
  if (_dirColID) {
    string dir = Controller::getConfig()->getChar(_dirColID);
    return (dir.find(':') == dir.npos) ? getKonnektPath() + dir : dir;
  }
  return "";
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
    IMLOG("[iPackageHandler::loadPackages()] Brak katalogów z pakietami !");
    return;
  }

  for (tParsers::iterator parser = _parsers.begin(); parser != _parsers.end(); parser++) {
    for (FindFile::tFoundFiles::iterator dir = dirs.begin(); dir != dirs.end(); dir++) {

      FindFileFiltered files(dir->getFilePath() + "\\*");
      files.setFileOnly();

      (*parser)->setDefinitionFilter(files);
      files.find();

      if (files.nothingFound() || files.found().empty()) {
        continue;
      }
      try {
        *this << &(*parser)->parse(files.found())->setDir(dir->getFileName());
      } catch (const Exception& e) {
        IMLOG("[iPackageHandler::loadPackages()] b³¹d podczas parsowania paczki (%s): %s", 
          dir->getFileName().c_str(), e.getReason().c_str());
      }
    }
  }
}