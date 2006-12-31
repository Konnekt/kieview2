/**
  *  Template parsing class handler
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 24 $
  *  @modifiedby   $LastChangedBy: dulek000 $
  *  @lastmodified $Date: 2006-12-16 15:09:18 +0100 (So, 16 gru 2006) $
  */

#include "stdafx.h"
#include "TplHandler.h"

std::string TplHandler::getTplPath(const char* tplName) {
  std::string fullPath = tplDir;

  fullPath += tplName;
  fullPath += ".";
  fullPath += tplExt;

  return fullPath;
}

String TplHandler::getTpl(const char* tplName) {
  loader_base loader;
  loader.load_file(getTplPath(tplName));

  return loader.get_data();
}

void TplHandler::bindStdFunctions() {
  using namespace template_parser_std_fn_ns;

  bindUdf("urlEscape", new urlescape());
  bindUdf("numFormat", new num_format());
  bindUdf("htmlEscape", new htmlescape());
  bindUdf("inSet", new value_in_set());
  bindUdf("hrefParam", new href_param());
  bindUdf("formParam", new form_param());
  bindUdf("isEmail", new udf_is_email());
  bindUdf("isAlpha", new udf_is_alpha());
  bindUdf("isAlNum", new udf_is_alnum());
  bindUdf("isNum", new udf_is_num());
  bindUdf("isInt", new udf_is_int());
  bindUdf("isFloat", new udf_is_float());
}

String TplHandler::parseException(const exception &e) {
  String result, exceptionString;

  param_data data(param_data::HASH);
  data.hash_insert_new_var("reason", e.what());

  exceptionString = 
    "<div class=\"exception\">"
      "<b>Exception caught</b> (<TMPL_var htmlEscape(reason)>) !"
    "</div>";

  try {
    result = parseTpl(&data, "exception");
  } catch(...) { 
    try {
      result = parseString(&data, exceptionString);
    } catch(const exception &e2) {
      result = "It can't be worse - exception caught while parsing exceptionString (";
      result += e2.what();
      result += ")<br/>";
    }
  }
  return result;
}

String TplHandler::parseString(param_data* data, const StringRef& text) {
  template_text parser(&udfFactory);
  String result;

  // Set allowed list of catalogs to include
  parser.set_include_dir(includeDir);
  // Parse the template
  parser.parse(text);
  // We impose parameters on a pattern
  parser.param(data);
  // Result
  result = parser.output();
  // clean
  // parser.clear_template();

  return result;
}

String TplHandler::parseTpl(param_data* data, const char* tplName) {
  return parseString(data, getTpl(tplName));
}