#pragma once

#ifndef __ISECTION_TOKEN_H__
#define __ISECTION_TOKEN_H__

#include "iTemplateToken.h"
#include "TemplateParser.h"

class iSectionToken: public iTemplateToken {
public:
  struct sSectionArg {
    union {
      string* name;
      UINT index;
    };
    TemplateParam* param;

    sSectionArg(string name, TemplateParam* param): name(new string(name)), param(param) { }
    sSectionArg(UINT index, TemplateParam* param): index(index), param(param) { }
  };

  enum enSectionType {
    tUnnamed = 1,
    tNamed = 2
  };

  typedef vector<sSectionArg*> tSectionArgs;

public:
  iSectionToken(TemplateParser* parser, iBlockToken* parent): iTemplateToken(parser, parent) { }
  virtual ~iSectionToken();

public:
  virtual enSectionType getSectionType() = 0;
  void parseArguments(string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);

protected:
  tSectionArgs _sectionArgs;
};

#endif // __ISECTION_TOKEN_H__