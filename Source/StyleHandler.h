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

#include <stamina/findfile.h>

#include "iPackageHandler.h"
#include "Emots.h"

class StyleHandler : public iPackageHandler {
public:
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(StyleHandler, iPackageHandler, Version(0,1,0,0));

public:
  typedef list<TplSet> tStyleSets;

public:
  StyleHandler() {
    _dirColID = kIEview2::cfg::stylesDir;
  }

public:
  void fillLV(iLV* lv);

  void clearPackages() {
    if (styleSets.size()) styleSets.clear();
  }
  void loadPackages();

  void loadSettings();
  void saveSettings();

  string getCurrentStyleDir();

protected:
  tStyleSets styleSets;
};

#endif // __STYLEHANDLER_H__