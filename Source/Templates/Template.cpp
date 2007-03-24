#include "stdafx.h"
#include "Template.h"
#include "TemplateParser.h"
#include "TemplateToken.h"

TemplateValue Template::get(const string& name) {
  return TemplateValue();
}

TemplateValue Template::call(const string& name, Template::enArgs& args, ...) {
  return TemplateValue();
}