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

  FindFile::tFoundFiles dirs;
  FindFile find;

  find.setMask(getDir() + "\\*");
  find.setDirOnly();

  dirs = find.makeList();

  if (find.nothingFound()) {
    IMLOG("[iPackageHandler::loadPackages()] Nie znaleziono katalogu z pakietami !");
    return;
  }
  if (!dirs.size()) {
    IMLOG("[iPackageHandler::loadPackages()] Brak katalogów z pakietami !");
    return;
  }

  for (tParsers::iterator it2 = _parsers.begin(); it2 != _parsers.end(); it2++) {
    for (FindFile::tFoundFiles::iterator it = dirs.begin(); it != dirs.end(); it++) {
      FindFile::tFoundFiles fileList;
      FindFileFiltered files;

      files.setMask(it->getFilePath() + "\\*");
      files.setFileOnly();

      (*it2)->setDefinitionFilter(files);
      fileList = files.makeList();

      try {
        if (files.nothingFound() || !fileList.size() || fileList.front().empty()) {
          continue;
        }
        addPackage(&(*it2)->parse(fileList.front())->setDir(it->getFileName()));
      } catch (const Exception& e) {
        IMLOG("[iPackageHandler::loadPackages()] b³¹d podczas parsowania paczki emot (%s): %s", 
          it->getFileName().c_str(), e.getReason().c_str());
      }
    }
  }
}