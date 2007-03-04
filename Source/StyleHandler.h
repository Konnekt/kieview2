/**
  *  Template parsing class handler
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 25 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-16 15:46:15 +0100 (So, 16 gru 2006) $
  */

#pragma once

#ifndef __TPLHANDLER_H__
#define __TPLHANDLER_H__

#include <ctpp/ctpp.hpp>
#include <functions/std_fn_list.hpp>

#include "kIEview2.h"
#include "iPackageHandler.h"
#include "Helpers.h"

#pragma comment(lib, "ctpp.lib")

using namespace template_parser_ns;
using namespace Stamina;
using namespace boost;
using namespace Helpers;

class StyleSet : public iPackage {
public:
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(StyleSet, iPackage, Version(0,1,0,0));

public:
  StyleSet(): _savable(true), _system(false) { }

public:
  virtual bool isSavable() const {
    return _savable;
  }
  virtual void isSavable(bool value) {
    _savable = value;
  }

  virtual bool isSystem() const {
    return _system;
  }
  virtual void isSystem(bool value) {
    _system = value;
  }

  virtual string getExt() {
    return _ext;
  }
  virtual void setExt(const string& newExt) {
    _ext = newExt;
  }

  virtual bool hasPreview() const {
    return _preview.length();
  }
  virtual string getPreview() {
    return _preview;
  }
  virtual void setPreview(const string& img_path) {
    _preview = fileExists(img_path.c_str()) ? img_path : "";
  }

protected:
  string _preview;
  string _ext;

  bool _savable;
  bool _system;
};

typedef SharedPtr<StyleSet> oStyleSet;

class TplPackageParser: public iPackageParser {
public:
  string getDefinitionMask() {
    return "template.xml";
  }
  string getArchiveMask() {
    return "*.ktpl";
  }
  bool fromArchive() {
    return true;
  }
  iPackage* parse(const FindFile::Found& defFile);
};

class TplHandler : public iPackageHandler {
public:
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(TplHandler, iPackageHandler, Version(0,1,0,0));

public:
  TplHandler();

  /* CTPP helpers */
  String runFunc(const string& name, udf_fn_param& params);
  String runFunc(const string& name, const StringRef& param1);
  String runFunc(const string& name, const StringRef& param1, const StringRef& param2);
  String runFunc(const string& name, const StringRef& param1, const StringRef& param2, const StringRef& param3);

  /* CTPP related */
  inline udf_fn_factory* getUdfFactory() {
    return &_udfFactory;
  }
  inline void bindUdf(const string& name, udf_fn* function) {
    getUdfFactory()->install_udf_fn(name, function);
  }
  void bindStdFunctions();

  /* iPackageHandler derived methods */
  void fillLV(iLV* lv);

  void loadPackages();
  void loadSettings();
  void saveSettings();

  /* Styleset handling */
  StyleSet* getByID(const string& id);
  StyleSet* getCurrentStyle();

  /* Styleset parsing */
  String parseString(param_data* data, const StringRef& text, StyleSet* styleSet);
  String parseTpl(param_data* data, const char* tplName, StyleSet* styleSet);
  String parseException(const exception &e, StyleSet* styleSet);

  /* directory paths obtaining related methods */
  inline string getDir() {
    return __super::getDir(kIEview2::cfg::stylesDir);
  }

  string getStyleDir(StyleSet* set);
  string getSystemStylesDir();

  inline void clearDirs() {
    _includeDirs.clear();
  }
  inline void addIncludeDir(const string& dir) {
    _includeDirs.push_back(dir);
  }

  string getTplPath(const char* tplName, StyleSet* styleSet);
  String getTpl(const char* tplName, StyleSet* styleSet);

protected:
  static const char _sysStylesPath[];
  StyleSet _emptySet;

  udf_fn_factory _udfFactory;
  v_include_dir _includeDirs;
};

#endif // __TPLHANDLER_H__