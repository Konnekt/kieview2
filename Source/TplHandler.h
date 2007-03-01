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

class TplSet : public iPackage {
public:
  TplSet(const StringRef& name, const string& dir, const string& version = "", const StringRef& description = ""):
    iPackage(name, dir, version, description), _system(false) { }
  TplSet(): _system(false) { }
  virtual ~TplSet() { }

public:
  bool isSystem() {
    return _system;
  }
  void isSystem(bool value) {
    _system = value;
  }

  virtual bool hasPreview() {
    return _preview.length();
  }
  virtual string getPreview() const {
    return _preview;
  }
  virtual void setPreview(const string& img_path) {
    _preview = fileExists(img_path.c_str()) ? img_path : "";
  }

protected:
  string _preview;
  bool _system;
};

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

class SystemStylesMissing : public ExceptionString {
public:
  SystemStylesMissing(): ExceptionString("Nie znaleziono katalogu z g��wnymi szablonami !") { }
};

class TplHandler : public iPackageHandler {
public:
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(TplHandler, iPackageHandler, Version(0,1,0,0));

public:
  typedef vector<string> tTplDirs;

public:
  TplHandler(const string& tplExt = "tpl");

public:
  void fillLV(iLV* lv);

  void loadPackages();
  void loadSettings();
  void saveSettings();

  string getSystemStylesDir();
  string getCurrentStyleDir();
  TplSet* getCurrentStyle();

public:
  inline void clearDirs() {
    _includeDirs.clear();
    _tplDirs.clear();
  }
  inline void addIncludeDir(const string& dir) {
    _includeDirs.push_back(dir);
  }
  void addTplDir(const string& dir, bool asInclude = true);

  inline void setTplExt(const string& ext) {
    _tplExt = ext;
  }

  inline void bindUdf(const string& name, udf_fn* function) {
    getUdfFactory()->install_udf_fn(name, function);
  }
  inline udf_fn_factory* getUdfFactory() {
    return &_udfFactory;
  }

  String runFunc(const string& name, udf_fn_param& params);
  String runFunc(const string& name, const StringRef& param1);
  String runFunc(const string& name, const StringRef& param1, const StringRef& param2);
  String runFunc(const string& name, const StringRef& param1, const StringRef& param2, const StringRef& param3);

  string getTplDir(const char* tplName);
  string getTplPath(const char* tplName);

  String getTpl(const char* tplName);
  void bindStdFunctions();

  String parseString(param_data* data, const StringRef& text);
  String parseTpl(param_data* data, const char* tplName);
  String parseException(const exception &e);

protected:
  static const char _sysStylesPath[];
  static const char _coreStylesDir[];

  udf_fn_factory _udfFactory;
  string _tplExt;

  v_include_dir _includeDirs;
  tTplDirs _tplDirs;
  TplSet _emptySet;
};

#endif // __TPLHANDLER_H__