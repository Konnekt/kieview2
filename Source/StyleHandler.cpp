/**
  *  kIEview2 Style handler class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 19 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-15 17:31:48 +0100 (Pt, 15 gru 2006) $
  */

#include "stdafx.h"
#include "Helpers.h"

#include "StyleHandler.h"
#include "Controller.h"

string StyleHandler::getCurrentStyleDir() {
  string currentStyle = Controller::getConfig()->getChar(cfg::currentStyle);

  for (tStyleSets::iterator it = styleSets.begin(); it != styleSets.end(); it++) {
    if (it->getDir() == currentStyle) return getDir() + "\\" + it->getDir();
  }
  return getKonnektPath() + "data\\templates\\core";
}

void StyleHandler::loadSettings() {
  string currentStyle = Controller::getConfig()->getChar(cfg::currentStyle);

  for (tStyleSets::iterator it = styleSets.begin(); it != styleSets.end(); it++) {
    if (it->getDir() == currentStyle) {
      it->setEnabled(true); break;
    }
  }
  Controller::getInstance()->getTplHandler()->clearDirs();
  Controller::getInstance()->getTplHandler()->addTplDir(getDir() + "/" + currentStyle + "/");
  Controller::getInstance()->getTplHandler()->addTplDir("data/templates/core/");
}

void StyleHandler::saveSettings() {
  for (tStyleSets::iterator it = styleSets.begin(); it != styleSets.end(); it++) {
    if (it->isEnabled()) {
      Controller::getConfig()->set(cfg::currentStyle, it->getDir()); break;
    }
  }
}

void StyleHandler::fillLV(iLV* _lv) {
  StyleLV* lv = (StyleLV*) _lv;
  for (tStyleSets::iterator it = styleSets.begin(); it != styleSets.end(); it++) {
    lv->addItem(new StyleLV::sStylePackInfo(it->isEnabled(), &*it));
  }
}

void StyleHandler::loadPackages() {
  clearPackages();

  string tplDir = Controller::getConfig()->getChar(cfg::stylesDir);

  FindFile find;
  find.setMask(tplDir + "\\*");
  find.setDirOnly();

  FindFile::tFoundFiles tplDirs = find.makeList();

  if (find.nothingFound()) {
    IMLOG("[StyleHandler::loadPackages()] Nie znaleziono katalogu z szablonami !");
    return;
  }
  if (!tplDirs.size()) {
    IMLOG("[StyleHandler::loadPackages()] Brak katalogów z szablonami !");
    return;
  }

  for (FindFile::tFoundFiles::iterator it = tplDirs.begin(); it != tplDirs.end(); it++) {
    styleSets.push_back(TplSet(it->getFileName(), it->getFileName()));
  }
}