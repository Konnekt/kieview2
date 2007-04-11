#pragma once

#ifndef __ISECTION_TOKEN_H__
#define __ISECTION_TOKEN_H__

#include "iTemplateToken.h"

class TemplateParam;

class iSectionToken : public iTemplateToken {
public:
  struct sSectionArgument {
    union {
      String* name;
      UINT index;
    };
    TemplateParam* param;

    sSectionArgument(const StringRef& name, TemplateParam* param): name(new String(name)), param(param) { }
    sSectionArgument(UINT index, TemplateParam* param): index(index), param(param) { }
  };

  enum enSectionType {
    tUnnamed,
    tNamed
  };

  typedef vector<sSectionArgument*> tSectionArguments;

public:
  iSectionToken(iBlockToken* parent = NULL) : iTemplateToken(parent) { }
  virtual ~iSectionToken();

public:
  virtual enSectionType getSectionType() = 0;

protected:
  tSectionArguments _sectionArguments;
};

#endif // __ISECTION_TOKEN_H__