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