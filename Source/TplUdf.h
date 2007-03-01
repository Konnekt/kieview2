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

#include <ctpp/ctpp.hpp>
#include <functions/std_fn_list.hpp>

#include <boost/format.hpp>
#include "Controller.h"

#pragma comment(lib, "ctpp.lib")

using namespace template_parser_ns;
using namespace Stamina;
using namespace boost;

class udf_get_cfg_setting: public udf_fn {
public:
  inline e_accept_params accept_params() {
    return ANY_PARAMS;
  }
  inline void param(udf_fn_param& params) {
    _def = params.size() > 1 ? params[1] : "";
    _name = params[0];
  }

  inline void handler() {
    if (_def != "!") {
      try {
        _result = Controller::getInstance()->getSettingStr(_name, tableConfig);
      } catch(...) {
        _result = _def;
      }
    } else {
      _result = Controller::getInstance()->getSettingStr(_name, tableConfig);
    }
  }
  inline string& result() {
    return _result;
  }

protected:
  string _result;
  string _name;
  string _def;
};

class udf_get_cnt_setting: public udf_get_cfg_setting {
public:
  inline void param(udf_fn_param& params) {
    if (params.size() < 2) {
      throw logic_error("udf_get_cnt_setting: you didn't provided required arguments");
    }
    _cnt = atoi(params[1].c_str());
    _def = params.size() > 2 ? params[2] : "";
    _name = params[0];

    if (!_cnt && (params[1] != "0")) {
      throw logic_error("udf_get_cnt_setting: contact id conversion failed");
    }
  }

  inline void handler() {
    if (_def != "!") {
      try {
        _result = Controller::getInstance()->getSettingStr(_name, tableContacts, _cnt);
      } catch(...) {
        _result = _def;
      }
    } else {
      _result = Controller::getInstance()->getSettingStr(_name, tableContacts, _cnt);
    }
  }

protected:
  tCntId _cnt;
};

class udf_nl2br: public udf_fn {
public:
  inline e_accept_params accept_params() {
    return ONE_PARAM;
  }
  inline void param(const string& str) {
    _str = str;
  }

  inline void handler() {
    _str = RegEx::doReplace("/\r?\n/m", "<br />\r\n", _str.c_str());
  }
  inline string& result() {
    return _str;
  }

protected:
  string _str;
};

class udf_stringf: public udf_fn {
public:
  inline e_accept_params accept_params() {
    return ANY_PARAMS;
  }
  inline void param(udf_fn_param& params) {
    _format = params[0];
    _params = params;
    _params.erase(_params.begin());
  }

  inline void handler() {
    boost::format formatter(_format);
    for (udf_fn_param::iterator it = _params.begin(); it != _params.end(); it++) {
      formatter % *it;
    }
    _result = formatter.str();
  }
  inline string& result() {
    return _result;
  }

protected:
  udf_fn_param _params;
  string _format;
  string _result;
};

class udf_strftime: public udf_fn {
public:
  inline e_accept_params accept_params() {
    return TWO_PARAMS;
  }
  inline void param(const string& param1, const string& param2) {
    _date = _atoi64(param2.c_str());
    _format = param1;
  }

  inline void handler() {
    if (!_date) {
      throw exception("udf_strftime: incorrect date format");
    }
    Date64 date;
    date = _date;

    _result = date.strftime(_format.c_str());
  }
  inline string& result() {
    return _result;
  }

protected:
  string _result;
  string _format;
  __int64 _date;
};

class udf_match: public udf_fn {
public:
  inline e_accept_params accept_params() {
    return TWO_PARAMS;
  }
  inline void param(const string& param1, const string& param2) {
    reg.match(param1.c_str(), param2.c_str());
  }

  inline void handler() {
    _result = reg.isMatched() ? "1" : "";
  }
  inline string& result() {
    return _result;
  }

protected:
  string _result;
  RegEx reg;
};

class udf_replace: public udf_fn {
public:
  inline e_accept_params accept_params() {
    return THREE_PARAMS;
  }
  inline void param(const string& param1, const string& param2, const string& param3) {
    reg.setPattern(param1);
    reg.setSubject(param3);

    _replace = param2;
  }

  inline void handler() {
    _result = reg.replace(_replace.c_str());
  }
  inline string& result() {
    return _result;
  }

protected:
  string _replace;
  string _result;
  RegEx reg;
};

class udf_get_ext_param: public udf_fn {
public:
  inline e_accept_params accept_params() {
    return TWO_PARAMS;
  }
  inline void param(const string& param1, const string& param2) {
    _name = param2;
    _ext = param1;
  }

  inline void handler() {
    _result = GetExtParam(_ext, _name);
  }
  inline string& result() {
    return _result;
  }

protected:
  string _name;
  string _ext;
  string _result;
};

#endif // __TPLUDF_H__