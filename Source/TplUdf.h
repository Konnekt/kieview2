/**
  *  User defined functions for CT++
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 25 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-16 15:46:15 +0100 (So, 16 gru 2006) $
  */

#pragma once

#ifndef __TPLUDF_H__
#define __TPLUDF_H__

#include <ctpp\ctpp.hpp>
#include <functions\std_fn_list.hpp>

#include <boost/format.hpp>

#pragma comment(lib, "ctpp.lib")

using namespace template_parser_ns;
using namespace Stamina;
using namespace boost;

class udf_strftime: public udf_fn {
public:
  inline e_accept_params accept_params() {
    return TWO_PARAMS;
  }
  inline void param(const std::string& param1, const std::string& param2) {
    _date = (__int64) atoi(param1.c_str());
    _format = param2;
  }

  inline void handler() {
    _result = _date.strftime(_format.c_str());
  }
  inline std::string& result() {
    return _result;
  }

protected:
	std::string _result;
	std::string _format;
	Date64 _date;
};

class udf_sprintf: public udf_fn {
public:
  inline e_accept_params accept_params() {
    return ANY_PARAMS;
  }
  inline void param(udf_fn_param& params) {
    _format = params.front();
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
  inline std::string& result() {
    return _result;
  }

protected:
	udf_fn_param _params;
	std::string _format;
	std::string _result;
};

#endif // __TPLUDF_H__