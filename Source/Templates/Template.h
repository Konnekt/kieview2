#pragma once

#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

class Template;
typedef SharedPtr<class Template> oTemplate;

#include <fstream>
#include <Stamina/Exception.h>
#include "TemplateValue.h"
#include "TemplateToken.h"
#include "TemplateVar.h"

class TemplateException: public ExceptionString {
public:
  TemplateException(const StringRef& reason): ExceptionString(reason) { }
};

class Template: public SharedObject<iSharedObject>, public iVariableManager {
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(Template, iSharedObject, Version(0,0,0,1));

  friend class TemplateParser;

public:
  Template(const string& path);
  ~Template();

public:
  TemplateParser* getParser();
  bool loaded();
  string output();
  void clear();

public:
 // TemplateValue getVariable(const string& name);
//  bool setVariable(const string& name, const TemplateValue& value, bool create = false);

private:
  TemplateParser* _parser;
  iBlockToken* _token;
  bool _loaded;
  string _data;
};

#endif // __TEMPLATE_H__