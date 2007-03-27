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
  virtual int getType() {
    return T_NONE;
  }
public:
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) = 0;
  virtual string output() = 0;
  virtual void clear() = 0;
};

class iSectionToken: public iTemplateToken {
public:
  struct sSectionArg {
    union {
      string name;
      UINT index;
    };
    TemplateParam* param;

    sSectionArg(string name, TemplateParam* param): name(name), param(param) { }
    sSectionArg(UINT index, TemplateParam* param): index(index), param(param) { }
  };

  enum enSectionType {
    tUnnamed = 1,
    tNamed = 2,
  };

  typedef vector<sSectionArg*> tSectionArgs;

public:
  virtual ~iSectionToken();

public:
  virtual enSectionType getSectionType();
  void parseArguments(string::iterator itCurrPos, string::iterator itEnd, string::iterator& itPos);

protected:
  tSectionArgs _sectionArgs;
};

iSectionToken::~iSectionToken() {
  for (tSectionArgs::iterator it = _sectionArgs.begin(); it != _sectionArgs.end(); it++) {
    delete (*it)->param;
    delete *it;
  }
}

class iBlockToken: public iTemplateToken {
public:
  static const int T_BLOCK = 0xFFF00000;

public:
  typedef vector<iTemplateToken*> tTokenList;

public:
  iBlockToken(iBlockToken* token);
  iBlockToken() {}

public:
  virtual int getType() {
    return T_BLOCK;
  }

  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens);

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
  typedef vector<iBlockToken*> tBlocks;

public:
  static const int T_IF = T_BLOCK | 15;

public:
  virtual int getType() {
    return T_IF;
  }
public:
  IFToken();
  ~IFToken();

  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens);

  virtual void add(iTemplateToken* token);
  virtual iTemplateToken* get(UINT id);
  virtual bool remove(UINT id);
  virtual UINT count();
  virtual void clear();

protected:
  tBlocks _blocks;
  TemplateParam* _param;
  int _active;
};

class UnLessToken: public iBlockToken {
public:
  static const int T_UNLESS = T_BLOCK | 20;

public:
  virtual int getType() {
    return T_UNLESS;
  }

public:
  UnLessToken();
  ~UnLessToken();

public:
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens);
  virtual string output();

public:
  TemplateParam* _param;
};

class ArgumentToken: public iTemplateToken {
public:
  static const int T_ARGUMENT = 30;

public:
  ArgumentToken();
  ~ArgumentToken();

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

class IncludeToken: public iTemplateToken {
public:
  static const int T_INCLUDE = 35;

public:
  IncludeToken();
  ~IncludeToken();

public:
  virtual int getType() {
    return T_INCLUDE;
  }
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens);
  virtual string output();
  virtual void clear();

public:
  Template* _tpl;
};




#endif // __TEMPLATE_TOKEN_H__