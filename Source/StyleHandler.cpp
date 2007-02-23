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

  for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
    TplSet* set = (TplSet*) *it;
    if (set->getDir() == currentStyle) return getDir() + "\\" + set->getDir();
  }
  return getKonnektPath() + "data\\templates\\core";
}

void StyleHandler::loadSettings() {
  string currentStyle = Controller::getConfig()->getChar(cfg::currentStyle);

  for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
    TplSet* set = (TplSet*) *it;
    if (set->getDir() == currentStyle) {
      set->setEnabled(true); break;
    }
  }
  Controller::getInstance()->getTplHandler()->clearDirs();
  Controller::getInstance()->getTplHandler()->addTplDir(getDir() + "/" + currentStyle + "/");
  Controller::getInstance()->getTplHandler()->addTplDir("data/templates/core/");
}

void StyleHandler::saveSettings() {
  for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
    TplSet* set = (TplSet*) *it;
    if (set->isEnabled()) {
      Controller::getConfig()->set(cfg::currentStyle, set->getDir()); break;
    }
  }
}

void StyleHandler::fillLV(iLV* _lv) {
  StyleLV* lv = (StyleLV*) _lv;
  for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
    TplSet* set = (TplSet*) *it;
    lv->addItem(new StyleLV::sStylePackInfo(set->isEnabled(), &*set));
  }
}