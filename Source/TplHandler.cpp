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

#include "Controller.h"
#include "TplHandler.h"
#include "TplUdf.h"

iPackage* TplPackageParser::parse(const FindFile::Found& defFile) {
  SXML xml;
  xml.loadFile(defFile.getFilePath().c_str());

  TplSet set;
  set.setName(xml.getText("template/meta/name"));
  set.setVersion(xml.getText("template/meta/version"));
  set.setDescription(xml.getText("template/meta/description"));

  string preview = xml.getText("template/meta/preview");
  if (preview.length()) {
    set.setPreview(defFile.getDirectory() + preview);
  }
  return new TplSet(set);
}

const char TplHandler::_sysStylesPath[] = "data/templates";
const char TplHandler::_coreStylesDir[] = "__core";

TplHandler::TplHandler(const string& tplExt) {
  _dirColID = kIEview2::cfg::stylesDir;
  _tplExt = tplExt;

  bindStdFunctions();
  bindUdf("getCntSetting", new udf_get_cnt_setting);
  bindUdf("getSetting", new udf_get_cfg_setting);

  bindUdf("formatString", new udf_stringf);
  bindUdf("formatTime", new udf_strftime);

  bindUdf("getExtParam", new udf_get_ext_param);
  bindUdf("nl2br", new udf_nl2br);
  bindUdf("replace", new udf_replace);
  bindUdf("match?", new udf_match);

  *this << new TplPackageParser;
}

void TplHandler::loadPackages() {
  iPackageHandler::loadPackages();
  if (_packages.size()) {
    return;
  }

  iPackageHandler::loadPackages(unifyPath(getSystemStylesDir()));
  if (!_packages.size()) {
    IMLOG("[TplHandler::loadPackages()] Brak katalogów ze stylami systemowymi !");
    return;
    // throw SystemStylesMissing();
  }

  for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
    ((TplSet*) *it)->isSystem(true);
  }
}

string TplHandler::getSystemStylesDir() {
  return getKonnektPath() + unifyPath(_sysStylesPath, true);
}

TplSet* TplHandler::getCurrentStyle() {
  string currentStyle = Controller::getConfig()->getChar(cfg::currentStyle);

  for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
    if ((*it)->getDir() == currentStyle) return (TplSet*) *it;
  }
  if (!_packages.size()) {
    return &_emptySet;
  }
  return (TplSet*) _packages.front();
}

string TplHandler::getCurrentStyleDir() {
  if (getCurrentStyle()->isSystem()) {
    return getSystemStylesDir() + getCurrentStyle()->getDir();
  }
  return getDir() + "\\" + getCurrentStyle()->getDir();
}

void TplHandler::loadSettings() {
  clearDirs();

  getCurrentStyle()->setEnabled(true);
  addTplDir(getCurrentStyleDir());

  if (getCurrentStyle()->getDir() != _coreStylesDir) {
    addTplDir(getSystemStylesDir() + _coreStylesDir);
  }
}

void TplHandler::saveSettings() {
  TplSet* selectedStyle = 0;

  for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
    if ((*it)->isEnabled()) {
      selectedStyle = (TplSet*) *it; break;
    }
  }
  if (selectedStyle && selectedStyle->getDir() != _coreStylesDir) {
    Controller::getConfig()->set(cfg::currentStyle, selectedStyle->getDir());
  }
}

void TplHandler::fillLV(iLV* _lv) {
  StyleLV* lv = (StyleLV*) _lv;
  for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
    lv->addItem(new StyleLV::sStylePackInfo((*it)->isEnabled(), (TplSet*) *it));
  }
}

void TplHandler::addTplDir(const string& dir, bool asInclude) {
  if (!dir.size()) return;

  string tplDir = (dir.find(':') == dir.npos) ? getKonnektPath() + dir : dir;
  tplDir = unifyPath(tplDir);

  if (asInclude) {
    addIncludeDir(tplDir);
  }
  _tplDirs.push_back(tplDir);
}

String TplHandler::runFunc(const string& name, udf_fn_param& params) {
  udf_fn* func = getUdfFactory()->get(name);
  func->param(params);
  func->handler();
  return func->result();
}

String TplHandler::runFunc(const string& name, const StringRef& param1) {
  udf_fn* func = getUdfFactory()->get(name);
  func->param(param1);
  func->handler();
  return func->result();
}

String TplHandler::runFunc(const string& name, const StringRef& param1, const StringRef& param2) {
  udf_fn* func = getUdfFactory()->get(name);
  func->param(param1, param2);
  func->handler();
  return func->result();
}

String TplHandler::runFunc(const string& name, const StringRef& param1, const StringRef& param2, const StringRef& param3) {
  udf_fn* func = getUdfFactory()->get(name);
  func->param(param1, param2, param3);
  func->handler();
  return func->result();
}

std::string TplHandler::getTplDir(const char* tplName) {
  if (!_tplDirs.size()) return "";

  string fileName = tplName;
  fileName += "." + _tplExt;

  for (tTplDirs::iterator it = _tplDirs.begin(); it != _tplDirs.end(); it++) {
    if (fileExists(((*it) + "\\" + fileName).c_str())) return *it;
  }
  throw std::logic_error("Cannot find file '" + fileName + "'");
}

std::string TplHandler::getTplPath(const char* tplName) {
  string fullPath = getTplDir(tplName) + "\\";

  fullPath += tplName;
  fullPath += ".";
  fullPath += _tplExt;

  return fullPath;
}

String TplHandler::getTpl(const char* tplName) {
  loader_base loader;
  loader.load_file(getTplPath(tplName));

  return loader.get_data();
}

void TplHandler::bindStdFunctions() {
  using namespace template_parser_std_fn_ns;

  bindUdf("numFormat", new num_format);
  bindUdf("urlEscape", new urlescape);
  bindUdf("htmlEscape", new htmlescape);
  bindUdf("hrefParam", new href_param);
  bindUdf("formParam", new form_param);

  bindUdf("inSet?", new value_in_set);
  bindUdf("isEmail?", new udf_is_email);
  bindUdf("isAlpha?", new udf_is_alpha);
  bindUdf("isAlphaNum?", new udf_is_alnum);
  bindUdf("isNum?", new udf_is_num);
  bindUdf("isInt?", new udf_is_int);
  bindUdf("isFloat?", new udf_is_float);
  bindUdf("isTrue?", new istrue);
}

String TplHandler::parseException(const exception &e) {
  String result, exceptionString;

  param_data data(param_data::HASH);
  data.hash_insert_new_var("reason", e.what());

  exceptionString = 
    "<div class=\"exception\">"
      "<b>Exception caught</b> ({{$htmlEscape(reason)}}) !"
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
  template_text parser(getUdfFactory());

  // Set allowed list of catalogs to include
  parser.set_include_dir(_includeDirs);
  // Parse the template
  parser.parse(text);
  // We impose parameters on a pattern
  parser.param(data);

  return parser.output();
}

String TplHandler::parseTpl(param_data* data, const char* tplName) {
  if (getKonnektPath().size()) {
    data->hash_insert_new_var("tplPath", getKonnektPath() + getTplDir(tplName));
  }
  return parseString(data, getTpl(tplName));
}
