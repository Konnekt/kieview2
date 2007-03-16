#pragma once

#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

#include <fstream>
#include "TemplateValue.h"

class TemplateException {
};

class Template {
public:
  Template(const string& path) {
    ifstream tpl(path.c_str());

    string data;
    if (!tpl.is_open())
    //  throw TemplateException("Template does not exist in path " + path);
    while (!tpl.eof()) {
      //tpl >> 
    }
    tpl.close();
    _token = new iBlockToken(NULL);
  }

  ~Template() {
    delete _token;
  }

public:
  static iTemplateValue* call(const string& name, enArgs& args, ...);
  static iTemplateValue* get(const string& name);
  static bool set(const string& name, const iTemplateValue& var);
  static bool set(const string& name, const iTemplateValue* var);

public:
  int line;
  int pos;
};





#endif // __TEMPLATE_H__