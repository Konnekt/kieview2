#include "stdafx.h"
#include "Template.h"
#include "TemplateParser.h"
#include "TemplateToken.h"

FileTemplate::FileTemplate(const string& path) {
  ifstream file(path.c_str());
  string buff;

  if (!file.is_open()) {
    throw TemplateException("Template file '" + path + "' does not exist");
  }
  while (!file.eof()) {
    getline(file, buff);
    _data += buff + '\n';
  }
  file.close();

  _loaded = true;
  _token = NULL;
}

string iTemplate::output() {
  return _token ? _token->output() : "";
}