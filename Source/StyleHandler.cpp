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
    *this << new StylePackageParser;

    bindStdFunctions();
  }

  void StyleHandler::bindStdFunctions() {
    // std
    Globals::get()->addFunction("html_escape", &funcs::html_escape);

    /*
    Globals::get()->addFunction("num_format", &funcs::num_format);
    Globals::get()->addFunction("url_escape", &funcs::url_escape);

    Globals::get()->addFunction("in_set", &funcs::in_set);
    Globals::get()->addFunction("is_email", &funcs::is_email);
    Globals::get()->addFunction("is_alpha", &funcs::is_alpha);
    Globals::get()->addFunction("is_alphanum", &funcs::is_alphanum);
    Globals::get()->addFunction("is_num", &funcs::is_num);
    Globals::get()->addFunction("is_int", &funcs::is_int);
    Globals::get()->addFunction("is_float", &funcs::is_float);
    */

    // K related
    Globals::get()->addFunction("get_ext_param", &funcs::get_ext_param);
    Globals::get()->addFunction("get_plugin_version", &funcs::get_plugin_version);
    Globals::get()->addFunction("get_plugin_name", &funcs::get_plugin_name);
    Globals::get()->addFunction("get_cnt_setting", &funcs::get_cnt_setting);
    Globals::get()->addFunction("get_cfg_setting", &funcs::get_cfg_setting);

    Globals::get()->addFunction("stringf", &funcs::stringf);
    Globals::get()->addFunction("timef", &funcs::strftime);

    Globals::get()->addFunction("nl2br", &funcs::nl2br);
    Globals::get()->addFunction("br2nl", &funcs::br2nl);

    Globals::get()->addFunction("html_unescape", &funcs::htmlunescape);
    Globals::get()->addFunction("replace", &funcs::replace);
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
    throw ExceptionString(stringf("Style set with id = '%s' wasn't found", id.c_str()));
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
    return stringf("%s\\%s.%s", styleSet->getDir().c_str(), 
      tplName, styleSet->getExt().length() 
        ? styleSet->getExt().c_str()
        : "tpl");
  }

  oTemplate StyleHandler::getTpl(const char* tplName, StyleSet* styleSet) {
    return new FileTemplate(getTplPath(tplName, styleSet), 0, CP_ANSI);
  }

  String StyleHandler::parseException(const Exception& e, StyleSet* styleSet) {
    oTemplate tpl;
    String result;

    try {
      tpl = getTpl("exception", styleSet);
      tpl->addVariable("reason", e.getReason());
    } catch (...) { 
      try {
        tpl = new TextTemplate(
          "<div class=\"exception\">"
            "<b>Exception caught</b> ({html_escape($reason)}) !"
          "</div>"
        );
        tpl->addVariable("reason", e.getReason());
      } catch (const Exception& e2) {
        return stringf("It can't be worse - multiple exception caught (%s)<br />", e2.getReason().c_str());
      }
    }
    Parser parser;
    parser.parse(tpl);

    return tpl->output();
  }

  String StyleHandler::parseTpl(oTemplate& tpl, StyleSet* styleSet) {
    try {
      Parser parser;
      parser.parse(tpl);

      return tpl->output();
    } catch (Exception& e) {
      return parseException(e, styleSet);
    }
  }
}