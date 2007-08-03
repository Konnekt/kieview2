/**
  *  User defined functions for CT++
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

#ifndef __TPLUDF_H__
#define __TPLUDF_H__

#include <boost/format.hpp>

#include "Helpers.h"
#include "Controller.h"

using namespace Stamina;
using namespace boost;

namespace kIEview2 { namespace funcs {
  inline oValue html_escape(const Globals::tFuncArguments& args) {
    string result, raw = args[0] >> String();

	  for (string::const_iterator ch(raw.begin()); ch != raw.end(); ++ch) {
		  switch (*ch) {
		  case '&':
			  result += "&amp;";
			  break;
		  case '<':
			  result += "&lt;";
			  break;
		  case '>':
			  result += "&gt;";
			  break;
		  case '"':
			  result += "&quot;";
			  break;
		  case '\'':
			  result += "&#039;";
			  break;
		  default:
			  result += *ch;
			  break;
		  }
	  }
	  return result;
  }

  inline oValue get_plugin_version(const Globals::tFuncArguments& args) {
    oValue v = args[0];
    int plugID = 0;

    if (v->isType<Values::Int>()) {
      plugID = Ctrl->ICMessage(IMC_FINDPLUG, v >> int(), IMT_ALL);
    } else {
      plugID = Ctrl->ICMessage(IMC_FINDPLUG_BYNAME, (int) (v >> String()).c_str());
    }
    if (plugID) {
      char ver[50] = {0};
      Ctrl->ICMessage(IMC_PLUG_VERSION, Ctrl->ICMessage(IMC_PLUGID_POS, plugID, 0), (int) ver);
      if (Ctrl->getError() != IMERROR_NORESULT) {
        return ver;
      }
    }
    throw SwiftException("Plugin not found");
  }
  inline oValue get_plugin_name(const Globals::tFuncArguments& args) {
    if (int plugID = pluginExists(args[0] >> int())) {
      return getPlugName(plugID);
    }
    throw SwiftException("Plugin not found");
  }
  inline oValue get_cfg_setting(const Globals::tFuncArguments& args) {
    string name = args[0] >> String();
    String def;

    if (args.size() > 1) {
      args[1] >> def;
    }
    if (def != "!") {
      try {
        return Controller::getInstance()->getSettingStr(name, tableConfig);
      } catch (...) {
        return def;
      }
    }
    return Controller::getInstance()->getSettingStr(name, tableConfig);
  }
  inline oValue get_cnt_setting(const Globals::tFuncArguments& args) {
    string name = args[0] >> String();
    tCntId cnt = args[1] >> int();
    String def;

    if (args.size() > 2) {
      args[2] >> def;
    }
    if (def != "!") {
      try {
        return Controller::getInstance()->getSettingStr(name, tableContacts, cnt);
      } catch(...) {
        return def;
      }
    }
    return Controller::getInstance()->getSettingStr(name, tableContacts, cnt);
  }
  inline oValue nl2br(const Globals::tFuncArguments& args) {
    return RegEx::doReplace("/\r?\n/m", "<br />\r\n", (args[0] >> String()).c_str());
  }
  inline oValue br2nl(const Globals::tFuncArguments& args) {
    return RegEx::doReplace("#<br ?/?>#i", "\r\n", (args[0] >> String()).c_str());
  }
  inline oValue stringf(const Globals::tFuncArguments& args) {
    boost::format formatter((args[0] >> String()).c_str());

    Globals::tFuncArguments _args = args;
    _args.erase(_args.begin());

    for each (const oValue& arg in _args) {
      formatter % (arg >> String()).c_str();
    }
    return formatter.str();
  }
  inline oValue strftime(const Globals::tFuncArguments& args) {
    __int64 _date = args[1] >> __int64();
    if (!_date) {
      throw SwiftException("Incorrect date format");
    }

    Date64 date;
    date = _date;

    return date.strftime((args[0] >> String()).c_str());
  }
  inline oValue get_ext_param(const Globals::tFuncArguments& args) {
    return GetExtParam(args[0] >> String(), args[1] >> String());
  }
  inline oValue replace(const Globals::tFuncArguments& args) {
    RegEx reg;
    reg.setPattern(args[0] >> String());
    reg.setSubject(args[2] >> String());

    return reg.replace((args[1] >> String()).c_str());
  }
  inline oValue htmlunescape(const Globals::tFuncArguments& args) {
    RegEx reg;
    reg.setSubject(args[0] >> String());

    reg.replaceItself("/&amp;/", "&");
    reg.replaceItself("/&lt;/", "<");
    reg.replaceItself("/&gt;/", ">");
    reg.replaceItself("/&quot;/", "\"");
    reg.replaceItself("/&(apos|#0?39);/", "'");

    return reg.getSubject();
  }
}}

#endif // __TPLUDF_H__