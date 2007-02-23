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

#pragma once

#ifndef __IPACKAGEHANDLER_H__
#define __IPACKAGEHANDLER_H__

#include "iPackage.h"
#include "iLV.h"

class iPackageHandler : public iObject {
public:
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(iPackageHandler, iObject, Version(0,1,0,0));

public:
  typedef list<iPackage*> tPackages;

public:
  iPackageHandler(): _dirColID(0) { }
  virtual ~iPackageHandler() {
    /*
    for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
      delete *it;
    }
    */
  }

public:
  virtual void fillLV(iLV* lv) = 0;

  virtual string getKonnektPath();
  virtual string getDir();

  virtual void clearPackages() = 0;
  /*
  virtual void clearPackages() {
    if (_packages.size()) _packages.clear();
  }
  */
  virtual void reloadPackages(iLV* lv = 0) {
    bool validLV = iLV::isValidLV(lv);

    if (validLV) lv->removeAll();
    loadPackages();
    loadSettings();
    if (validLV) fillLV(lv);
  }
  virtual void loadPackages() = 0;

  virtual void loadSettings() = 0;
  virtual void saveSettings() = 0;

protected:
  Tables::tColId _dirColID;
  // tPackages _packages;
};

#endif // __IPACKAGEHANDLER_H__