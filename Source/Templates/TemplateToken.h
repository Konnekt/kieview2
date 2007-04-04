#pragma once

#ifndef __TEMPLATE_TOKEN_H__
#define __TEMPLATE_TOKEN_H__

class iBlockToken;

#include "Template.h"
#include "TemplateValue.h"

class iTemplateToken {
public:
  static const int T_NONE = 1;

public:
  iTemplateToken(oTemplate& tpl): _tpl(tpl) { }

public:
  virtual int getType() {
    return T_NONE;
  }
  virtual oTemplate getTemplate() {
    return _tpl;
  }
public:
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) = 0;
  virtual string output() = 0;
  virtual void clear() = 0;

public:
  oTemplate _tpl;
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
  iSectionToken(oTemplate& tpl): iTemplateToken(tpl) { }
  virtual ~iSectionToken();

public:
  virtual enSectionType getSectionType() = 0;
  void parseArguments(string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);

protected:
  tSectionArgs _sectionArgs;
};


class iBlockToken: public iSectionToken {
public:
  static const int T_BLOCK = 0xFFF00000;

public:
  typedef vector<iTemplateToken*> tTokenList;

public:
  //iBlockToken(iBlockToken* token);
  iBlockToken(oTemplate& tpl): iSectionToken(tpl) { }

public:
  virtual int getType() {
    return T_BLOCK;
  }

  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens);
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

class TextToken: public iTemplateToken {
public:
  static const int T_TEXT = 10;

public:
  TextToken(oTemplate& tpl): iTemplateToken(tpl) { }

public:
  virtual int getType() {
    return T_TEXT;
  }

  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens);
  virtual string output();
  virtual void clear();

protected:
  string _text;
};


class IFToken: public iBlockToken {
public:
  static const int T_IF = T_BLOCK | 15;

public:
  virtual int getType() {
    return T_IF;
  }

public:
  IFToken(oTemplate& tpl);
  virtual ~IFToken();

  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens);

  virtual void add(iTemplateToken* token);
  virtual iTemplateToken* get(UINT id);
  virtual bool remove(UINT id);
  virtual UINT count();
  virtual void clear();
  virtual string output();

protected:
  iBlockToken* _ifBlock;
  iBlockToken* _elseBlock;
  int _active;
};

class UnLessToken: public iBlockToken {
public:
  static const int T_UNLESS = T_BLOCK | 20;

public:
  UnLessToken(oTemplate& tpl): iBlockToken(tpl) { }

public:
  virtual int getType() {
    return T_UNLESS;
  }

public:
  iSectionToken::enSectionType getSectionType();
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens);
  virtual string output();
};

class ArgumentToken: public iTemplateToken {
public:
  static const int T_ARGUMENT = 30;

public:
  ArgumentToken(oTemplate& tpl);
  virtual ~ArgumentToken();

public:
  virtual int getType() {
    return T_ARGUMENT;
  }
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens);
  virtual string output();
  virtual void clear();

public:
  TemplateParam* _param;
};

class IncludeToken: public iSectionToken {
public:
  static const int T_INCLUDE = 35;

public:
  IncludeToken(oTemplate& tpl);
  virtual ~IncludeToken();

public:
  virtual int getType() {
    return T_INCLUDE;
  }
  iSectionToken::enSectionType getSectionType();
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens);
  virtual string output();
  virtual void clear();

public:
  oTemplate _includeTpl;
};

class SetToken: public iSectionToken {
public:
  static const int T_SET = 40;

public:
  SetToken(oTemplate& tpl): iSectionToken(tpl) { }

public:
  virtual int getType() {
    return T_SET;
  }
  iSectionToken::enSectionType getSectionType();
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens);
  virtual string output();
  virtual void clear();
};


#endif // __TEMPLATE_TOKEN_H__