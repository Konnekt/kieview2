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

class iPackageParser {
public:
  virtual void setDefinitionFilter(FindFileFiltered& files) = 0;
  virtual iPackage* parse(const FindFile::Found& defFile) = 0;
};

class iPackageHandler : public iObject {
public:
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(iPackageHandler, iObject, Version(0,1,0,0));

public:
  typedef list<iPackageParser*> tParsers;
  typedef list<iPackage*> tPackages;

public:
  iPackageHandler(): _dirColID(0) { }
  virtual ~iPackageHandler() {
    for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
      delete *it;
    }
    for (tParsers::iterator it = _parsers.begin(); it != _parsers.end(); it++) {
      delete *it;
    }
  }

public:
  iPackageHandler& operator << (iPackageParser* parser) {
    addParser(parser);
    return *this;
  }
  iPackageHandler& operator << (iPackage* package) {
    addPackage(package);
    return *this;
  }

public:
  virtual void fillLV(iLV* lv) = 0;

  virtual string getKonnektPath();
  virtual string getDir();

  virtual void addParser(iPackageParser* parser) {
    _parsers.push_back(parser);
  }
  virtual void addPackage(iPackage* package) {
    _packages.push_back(package);
  }

  virtual void clearPackages() {
    if (_packages.size()) _packages.clear();
  }
  virtual void reloadPackages(iLV* lv = 0) {
    bool validLV = iLV::isValidLV(lv);

    if (validLV) lv->removeAll();
    loadPackages();
    loadSettings();
    if (validLV) fillLV(lv);
  }
  virtual void loadPackages();

  virtual void loadSettings() = 0;
  virtual void saveSettings() = 0;

protected:
  Tables::tColId _dirColID;
  tPackages _packages;
  tParsers _parsers;
};

#endif // __IPACKAGEHANDLER_H__