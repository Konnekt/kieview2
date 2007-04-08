#include "stdafx.h"
#include "Template.h"

FileTemplate::FileTemplate(const string& path, iVariableManager* vm): iTemplate(vm) {
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

String iTemplate::output() {
  return _token ? _token->output() : String();
}