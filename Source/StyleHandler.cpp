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

namespace kIEview2 {
  iPackage* StylePackageParser::parse(const FindFile::Found& defFile) {
    SXML xml;
    if (!xml.loadFile(defFile.getFilePath().c_str())) {
      throw ExceptionString("Brak pliku z definicj¹ szablonu");
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

  const char StyleHandler::_sysStylesPath[] = "data/templates";

  StyleHandler::StyleHandler() {
    bindStdFunctions();
    bindUdf("getExtParam", new udf_get_ext_param);

    bindUdf("getPluginVersion", new udf_get_plugin_version);
    bindUdf("getPluginName", new udf_get_plugin_name);

    bindUdf("getCntSetting", new udf_get_cnt_setting);
    bindUdf("getSetting", new udf_get_cfg_setting);

    bindUdf("formatString", new udf_stringf);
    bindUdf("formatTime", new udf_strftime);

    bindUdf("htmlUnescape", new udf_htmlunescape);
    bindUdf("nl2br", new udf_nl2br);
    bindUdf("br2nl", new udf_br2nl);
    bindUdf("replace", new udf_replace);
    bindUdf("match?", new udf_match);

    *this << new StylePackageParser;
  }

  String StyleHandler::runFunc(const string& name, udf_fn_param& params) {
    udf_fn* func = getUdfFactory()->get(name);
    func->param(params);
    func->handler();
    return func->result();
  }

  String StyleHandler::runFunc(const string& name, const StringRef& param1) {
    udf_fn* func = getUdfFactory()->get(name);
    func->param(param1);
    func->handler();
    return func->result();
  }

  String StyleHandler::runFunc(const string& name, const StringRef& param1, const StringRef& param2) {
    udf_fn* func = getUdfFactory()->get(name);
    func->param(param1, param2);
    func->handler();
    return func->result();
  }

  String StyleHandler::runFunc(const string& name, const StringRef& param1, const StringRef& param2, const StringRef& param3) {
    udf_fn* func = getUdfFactory()->get(name);
    func->param(param1, param2, param3);
    func->handler();
    return func->result();
  }

  void StyleHandler::bindStdFunctions() {
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
  }

  void StyleHandler::fillLV(iLV* _lv) {
    StyleLV* lv = (StyleLV*) _lv;
    for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
      lv->addItem(new StyleLV::sStylePackInfo((*it)->isEnabled(), (StyleSet*) *it));
    }
  }

  void StyleHandler::loadPackages() {
    __super::loadPackages();
    if (_packages.size()) {
      Controller::getInstance()->onStylesReload();
      return;
    }

    __super::loadPackages(unifyPath(getSystemStylesDir()));
    if (!_packages.size()) {
      IMLOG("[StyleHandler::loadPackages()] Brak katalogów ze stylami systemowymi !");
    } else {
      for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
        ((StyleSet*)*it)->isSystem(true);
      }
    }
    Controller::getInstance()->onStylesReload();
  }

  void StyleHandler::loadSettings() {
    getCurrentStyle()->setEnabled(true);
  }

  void StyleHandler::saveSettings() {
    StyleSet* selectedStyle = 0;

    for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
      if ((*it)->isEnabled()) {
        selectedStyle = (StyleSet*) *it; break;
      }
    }
    if (selectedStyle) {
      StyleSet* currentStyle = getCurrentStyle();
      if (selectedStyle->isSavable()) {
        Controller::getConfig()->set(cfg::currentStyle, selectedStyle->getID());
      }
      if (selectedStyle != currentStyle) {
        Controller::getInstance()->onSwitchStyle(currentStyle, selectedStyle);
      }
    }
  }

  StyleSet* StyleHandler::getByID(const string& id) {
    for (tPackages::iterator it = _packages.begin(); it != _packages.end(); it++) {
      if ((*it)->getID() == id) return (StyleSet*) *it;
    }
    throw ExceptionString("Style set with id = '" + id + "' wasn't found");
  }

  StyleSet* StyleHandler::getCurrentStyle() {
    try {
      return getByID(Controller::getConfig()->getString(cfg::currentStyle));
    } catch(...) {
      if (_packages.size()) {
        return (StyleSet*) _packages.front();
      }
    }
    return &_emptySet;
  }

  string StyleHandler::getSystemStylesDir() {
    return getKonnektPath() + unifyPath(_sysStylesPath, true);
  }

  string StyleHandler::getTplPath(const char* tplName, StyleSet* styleSet) {
    string fullPath = styleSet->getDir() + "\\";

    fullPath += tplName;
    fullPath += ".";
    fullPath += styleSet->getExt().length() ? styleSet->getExt() : "tpl";

    return fullPath;
  }

  String StyleHandler::getTpl(const char* tplName, StyleSet* styleSet) {
    loader_base loader;
    loader.load_file(getTplPath(tplName, styleSet));

    return loader.get_data();
  }

  String StyleHandler::parseException(const exception &e, StyleSet* styleSet) {
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

  String StyleHandler::parseString(param_data* data, const StringRef& text, StyleSet* styleSet) {
    if (!text.length()) {
      throw exception("No input text to parse");
    }
    template_text parser(getUdfFactory());

    // Set allowed list of catalogs to include
    parser.set_include_dir(_includeDirs);
    // Parse the template
    parser.parse(text);
    // We impose parameters on a pattern
    parser.param(data);

    return parser.output();
  }

  String StyleHandler::parseTpl(param_data* data, const char* tplName, StyleSet* styleSet) {
    data->hash_insert_new_var("tplPath", getTplPath(tplName, styleSet));
    return parseString(data, getTpl(tplName, styleSet), styleSet);
  }
}