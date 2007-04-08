#pragma once

#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

#include <fstream>
#include <Stamina/Exception.h>

#include "iBlockToken.h"

class TemplateException: public ExceptionString {
public:
  TemplateException(const StringRef& reason): ExceptionString(reason) { }
};

class iTemplate: public SharedObject<iSharedObject>, public iVariableManager {
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(iTemplate, iSharedObject, Version(0,0,0,1));

public:
  friend class TemplateParser;

public:
  virtual ~iTemplate() {
    clear();
  }

public:
  TemplateParser* getParser() {
    return _parser;
  }
  virtual bool loaded() {
    return _loaded;
  }
  virtual void clear() {
    if (_token) delete _token;
  }
  virtual string output();

public:
  // TemplateValue getVariable(const string& name);
  // bool setVariable(const string& name, const TemplateValue& value, bool create = false);

protected:
  TemplateParser* _parser;
  iBlockToken* _token;
  bool _loaded;
  string _data;
};

typedef SharedPtr<iTemplate> oTemplate;

class FileTemplate : public iTemplate {
public:
  FileTemplate(const string& path);
};

#endif // __TEMPLATE_H__