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

#pragma comment(lib, "ctpp.lib")

using namespace template_parser_ns;
using namespace Stamina;
using namespace boost;

class TplHandler : public SharedObject<iSharedObject> {
public:
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(TplHandler, iSharedObject, Version(0,1,0,0));

public:
  TplHandler(const string& tplDir = "", const string& tplExt = "tpl") {
    setTplDir(tplDir);
    setTplExt(tplExt);
  }

public:
  inline void addIncludeDir(const string& dir) {
    includeDir.push_back(dir);
  }
  inline void bindUdf(const string& name, udf_fn* function) {
    udfFactory.install_udf_fn(name, function);
  }

  inline void setTplDir(const string& dir) {
    tplDir = dir;
  }
  inline void setTplExt(const string& ext) {
    tplExt = ext;
  }

  inline udf_fn_factory* getUdfFactory() {
    return &udfFactory;
  }

  String runFunc(const string& name, udf_fn_param& params);
  String runFunc(const string& name, const StringRef& param1);
  String runFunc(const string& name, const StringRef& param1, const StringRef& param2);
  String runFunc(const string& name, const StringRef& param1, const StringRef& param2, const StringRef& param3);

  std::string getTplPath(const char* tplName);
  String getTpl(const char* tplName);
  void bindStdFunctions();

  String parseString(param_data* data, const StringRef& text);
  String parseTpl(param_data* data, const char* tplName);
  String parseException(const exception &e);

protected:
  udf_fn_factory udfFactory;
  v_include_dir includeDir;

  string tplDir;
  string tplExt;
};

typedef SharedPtr<TplHandler> oTplHandler;

#endif // __TPLHANDLER_H__