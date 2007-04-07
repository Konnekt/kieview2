#pragma once

#ifndef __TEMPLATE_TOKEN_H__
#define __TEMPLATE_TOKEN_H__

class iBlockToken;
class iTemplateToken;

#include "TemplateParser.h"
#include "TemplateValue.h"
#include "Template.h"
#include "TemplateVar.h"

class TemplateParser;
class iBlockToken;

class iTemplateToken {
public:
  static const int T_NONE = 1;

public:
  iTemplateToken(TemplateParser* parser, iBlockToken* parent): _parser(parser), _parent(parent) { }

public:
  virtual TemplateParser* getParser() {
    return _parser;
  }
  virtual iBlockToken* getParent() {
    return _parent;
  }
  virtual int getType() {
    return T_NONE;
  }

public:
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, 
    string::iterator& itPos, bool allowCreateTokens) = 0;
  virtual string output() = 0;
  virtual void clear() = 0;

public:
  TemplateParser* _parser;
  iBlockToken* _parent;
};

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
    tNamed = 2,
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

class iBlockToken: public iSectionToken, public iVariableManager {
public:
  static const int T_BLOCK = 0xFFF00000;

public:
  typedef vector<iTemplateToken*> tTokenList;

public:
  // iBlockToken(iBlockToken* token);
  iBlockToken(TemplateParser* parser, iBlockToken* parent): iSectionToken(parser, parent) { }

public:
  virtual int getType() {
    return T_BLOCK;
  }

  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, 
    string::iterator& itPos, bool allowCreateTokens);
  virtual iSectionToken::enSectionType getSectionType();

  virtual void add(iTemplateToken* token);
  virtual iTemplateToken* get(UINT id);
  virtual bool remove(UINT id);
  virtual UINT count();
  virtual void clear();

  virtual string output();

protected:
  tTokenList _token;
};

#endif // __TEMPLATE_TOKEN_H__