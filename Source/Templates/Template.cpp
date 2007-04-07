#include "stdafx.h"
#include "Template.h"
#include "TemplateParser.h"
#include "TemplateToken.h"

Template::Template(const string& path) {
  ifstream tpl(path.c_str());

  string line;
  if (!tpl.is_open()) {
    throw TemplateException("Template does not exist in path: " + path);
  }
  while (!tpl.eof()) {
    getline(tpl, line);
    _data += line + '\n';
  }
  tpl.close();
  _token = NULL;

  _loaded = true;
}

Template::~Template() {
  clear();
}

bool Template::loaded() {
  return _loaded;
}

void Template::clear() {
  if (_token) {
    delete _token;
  }
}

TemplateParser* Template::getParser() {
  return _parser;
}

string Template::output() {
  return _token->output();
}