#include "stdafx.h"
#include "Template.h"
#include "TemplateParser.h"
#include "TemplateToken.h"


Template::Template(const string& path) {
  ifstream tpl(path.c_str());

  string data;
  if (!tpl.is_open()) {
    throw TemplateException("Template does not exist in path " + path);
  }
  while (!tpl.eof()) {
    tpl >> data;
  }
  tpl.close();
  _token = new iBlockToken();
  _loaded = true;
}

Template::~Template() {
  delete _token;
}

bool Template::loaded() {
  return _loaded;
}

string Template::output() {
  return _token->output();
}

TemplateValue Template::get(const string& name) {
  return TemplateValue();
}

TemplateValue Template::call(const string& name, Template::enArgs& args, ...) {
  return TemplateValue();
}