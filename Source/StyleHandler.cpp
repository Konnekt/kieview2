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
#include "StyleHandler.h"
#include "TplUdf.h"

iPackage* TplPackageParser::parse(const FindFile::Found& defFile) {
  SXML xml;
  if (!xml.loadFile(defFile.getFilePath().c_str())) {
    throw ExceptionString("Brak pliku z definicj� szablonu");
  }

  string buff;
  StyleSet set;

  buff = xml.getText("template/meta/name");
  if (!buff.length()) {
    throw ExceptionString("Brak nazwy szablonu");
  }
  set.setName(buff);

  set.setID(xml.getText("template/meta/id"));
  set.setVersion(xml.getText("template/meta/version"));
  set.setDescription(xml.getText("template/meta/description"));
  set.setExt(xml.getText("template/options/ext"));

  buff = xml.getText("template/options/noSave");
  set.isSavable(!buff.length() || buff == "false");

  buff = xml.getText("template/meta/preview");
  set.setPreview(buff.length() ? defFile.getDirectory() + buff : "");

  return new StyleSet(set);
}

const char TplHandler::_sysStylesPath[] = "data/templates";

TplHandler::TplHandler() {
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

void TplHandler::fillLV(iLV* _lv) {
  StyleLV* lv = (StyleLV*) _lv;
  for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
    lv->addItem(new StyleLV::sStylePackInfo((*it)->isEnabled(), (StyleSet*) *it));
  }
}

void TplHandler::loadPackages() {
  __super::loadPackages();
  if (_packages.size()) {
    return;
  }

  __super::loadPackages(unifyPath(getSystemStylesDir()));
  if (!_packages.size()) {
    IMLOG("[TplHandler::loadPackages()] Brak katalog�w ze stylami systemowymi !");
    return;
  }

  for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
    ((StyleSet*)*it)->isSystem(true);
  }
}

void TplHandler::loadSettings() {
  getCurrentStyle()->setEnabled(true);
}

void TplHandler::saveSettings() {
  StyleSet* selectedStyle;

  for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
    if ((*it)->isEnabled()) {
      selectedStyle = (StyleSet*) *it; break;
    }
  }
  if (selectedStyle && selectedStyle->isSavable()) {
    StyleSet* currentStyle = getCurrentStyle();
    if (selectedStyle != currentStyle) {
      Controller::getConfig()->set(cfg::currentStyle, selectedStyle->getID());
      Controller::getInstance()->switchStyle(currentStyle, selectedStyle);
    }
  }
}

StyleSet* TplHandler::getByID(const string& id) {
  for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
    if ((*it)->getID() == id) return (StyleSet*) *it;
  }
  return NULL;
}

StyleSet* TplHandler::getCurrentStyle() {
  StyleSet* currentStyle = getByID(Controller::getConfig()->getString(cfg::currentStyle));

  if (currentStyle) {
    return currentStyle;
  }
  if (_packages.size()) {
    return (StyleSet*) _packages.front();
  }
  return &_emptySet;
}

string TplHandler::getStyleDir(StyleSet* set) {
  if (set->isSystem()) {
    return getSystemStylesDir() + set->getDir();
  }
  return getDir() + "\\" + set->getDir();
}

string TplHandler::getSystemStylesDir() {
  return getKonnektPath() + unifyPath(_sysStylesPath, true);
}

string TplHandler::getTplPath(const char* tplName, StyleSet* styleSet) {
  string fullPath = getStyleDir(styleSet) + "\\";

  fullPath += tplName;
  fullPath += ".tpl";

  return fullPath;
}

String TplHandler::getTpl(const char* tplName, StyleSet* styleSet) {
  loader_base loader;
  loader.load_file(getTplPath(tplName, styleSet));

  return loader.get_data();
}

String TplHandler::parseException(const exception &e, StyleSet* styleSet) {
  String result, exceptionString;

  param_data data(param_data::HASH);
  data.hash_insert_new_var("reason", e.what());

  exceptionString = 
    "<div class=\"exception\">"
      "<b>Exception caught</b> ({{$htmlEscape(reason)}}) !"
    "</div>";

  try {
    result = parseTpl(&data, "exception", styleSet);
  } catch(...) { 
    try {
      result = parseString(&data, exceptionString, styleSet);
    } catch(const exception &e2) {
      result = "It can't be worse - exception caught while parsing exceptionString (";
      result += e2.what();
      result += ")<br/>";
    }
  }
  return result;
}

String TplHandler::parseString(param_data* data, const StringRef& text, StyleSet* styleSet) {
  template_text parser(getUdfFactory());

  // Set allowed list of catalogs to include
  parser.set_include_dir(_includeDirs);
  // Parse the template
  parser.parse(text);
  // We impose parameters on a pattern
  parser.param(data);

  return parser.output();
}

String TplHandler::parseTpl(param_data* data, const char* tplName, StyleSet* styleSet) {
  data->hash_insert_new_var("tplPath", getTplPath(tplName, styleSet));
  return parseString(data, getTpl(tplName, styleSet), styleSet);
}
