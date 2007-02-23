/**
  *  kIEview2 Style handler class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 43 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-31 16:47:02 +0100 (N, 31 gru 2006) $
  */
  
#pragma once

#ifndef __STYLEHANDLER_H__
#define __STYLEHANDLER_H__

#include "iPackageHandler.h"

class TplSet : public iPackage {
public:
  TplSet(const StringRef& name, const string& dir, const string& version = "", const StringRef& description = ""):
    iPackage(name, dir, version, description) { }
  TplSet() { }
  virtual ~TplSet() { }
};

class StyleHandler : public iPackageHandler {
public:
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(StyleHandler, iPackageHandler, Version(0,1,0,0));

public:
  StyleHandler() {
    _dirColID = kIEview2::cfg::stylesDir;
  }

public:
  void fillLV(iLV* lv);

  void loadSettings();
  void saveSettings();

  string getCurrentStyleDir();
};

#endif // __STYLEHANDLER_H__