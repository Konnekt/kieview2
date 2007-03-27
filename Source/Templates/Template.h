#pragma once

#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

class Template;

#include <fstream>
#include "TemplateValue.h"
#include "TemplateToken.h"
#include <Stamina/Exception.h>

class TemplateException: public ExceptionString {
public:
  TemplateException(const StringRef& reason): ExceptionString(reason) { }
};

class Template {
public:
  enum enArgs {
    argsZero = 0,
    argsOne = 1,
    argsTwo = 2,
    argsThree = 3,
    argsFour = 4,
  };

public:
  Template(const string& path);
  ~Template();

public:
  static TemplateValue call(const string& name, enArgs& args, ...);
  static TemplateValue get(const string& name);
  static bool set(const string& name, const TemplateValue* var);

public:
  bool loaded();
  string output();

private:
  iBlockToken* _token;
  bool _loaded;
};

#endif // __TEMPLATE_H__