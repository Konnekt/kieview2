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
#include "oZip.h"
#include "iLV.h"

using namespace Konnekt::Tables;

class iPackageParser {
public:
  virtual bool fromArchive() const { return false; }

  virtual string getDefinitionMask() const = 0;
  virtual string getArchiveMask() const { return ""; }

  virtual iPackage* parse(const FindFile::Found& defFile) = 0;
};

class iPackageHandler : public iObject {
public:
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(iPackageHandler, iObject, Version(0,1,0,0));

public:
  typedef vector<iPackageParser*> tParsers;
  typedef list<iPackage*> tPackages;

public:
  iPackageHandler() { }
  virtual ~iPackageHandler() {
    for (tParsers::iterator it = _parsers.begin(); it != _parsers.end(); it++) {
      delete *it;
    }
    clearPackages();
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

  virtual void addParser(iPackageParser* parser) {
    _parsers.push_back(parser);
  }
  virtual void addPackage(iPackage* package) {
    for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
      if ((*it)->getID() == package->getID()) {
        throw ExceptionString("Package with id = '" + package->getID() + "' already exists");
      }
    }
    _packages.push_back(package);
  }

public:
  static string getKonnektPath();
  virtual string getDir() = 0;

  virtual string getRepoPath(const string& path); 
  virtual void prepareRepo(const string& path); 

  virtual void fillLV(iLV* lv) = 0;

  virtual void load() {
    loadPackages();
    loadSettings();
  }
  virtual void reload(iLV* lv = 0) {
    bool validLV = iLV::isValidLV(lv);

    if (validLV) lv->removeAll();
    load();
    if (validLV) fillLV(lv);
  }

  virtual void clearPackages() {
    for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
      delete *it;
    }
    _packages.clear();
  }
  virtual void loadPackages() {
    loadPackages(getDir());
  }

  virtual void loadSettings() = 0;
  virtual void saveSettings() = 0;

protected:
  virtual string getDir(tColId dirColID);

  virtual iPackage* loadPackage(iPackageParser* parser, FindFile::Found& dir);
  virtual void loadPackages(const string& dir);

protected:
  tPackages _packages;
  tParsers _parsers;
};

#endif // __IPACKAGEHANDLER_H__
