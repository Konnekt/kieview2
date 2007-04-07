#pragma once

#ifndef __TEMPLATE_TOKENS
#define __TEMPLATE_TOKENS

#include "TemplateToken.h"

class TextToken: public iTemplateToken {
public:
  static const int T_TEXT = 10;

public:
  TextToken(TemplateParser* parser, iBlockToken* parent): iTemplateToken(parser, parent) { }

public:
  virtual int getType() {
    return T_TEXT;
  }
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, 
    string::iterator& itPos, bool allowCreateTokens);
  virtual string output();
  virtual void clear();

protected:
  string _text;
};

class IFToken: public iBlockToken {
public:
  static const int T_IF = T_BLOCK | 15;

public:
  IFToken(TemplateParser* parser, iBlockToken* parent);
  virtual ~IFToken();

public:
  virtual int getType() {
    return T_IF;
  }
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, 
    string::iterator& itPos, bool allowCreateTokens);

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
  UnLessToken(TemplateParser* parser, iBlockToken* parent): iBlockToken(parser, parent) { }

public:
  virtual int getType() {
    return T_UNLESS;
  }
  iSectionToken::enSectionType getSectionType();
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, 
    string::iterator& itPos, bool allowCreateTokens);
  virtual string output();
};

class ArgumentToken: public iTemplateToken {
public:
  static const int T_ARGUMENT = 30;

public:
  ArgumentToken(TemplateParser* parser, iBlockToken* parent);
  virtual ~ArgumentToken();

public:
  virtual int getType() {
    return T_ARGUMENT;
  }
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, 
    string::iterator& itPos, bool allowCreateTokens);
  virtual string output();
  virtual void clear();

public:
  TemplateParam* _param;
};

class IncludeToken: public iSectionToken {
public:
  static const int T_INCLUDE = 35;

public:
  IncludeToken(TemplateParser* parser, iBlockToken* parent);
  virtual ~IncludeToken();

public:
  virtual int getType() {
    return T_INCLUDE;
  }
  iSectionToken::enSectionType getSectionType();
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, 
    string::iterator& itPos, bool allowCreateTokens);
  virtual string output();
  virtual void clear();

public:
  oTemplate _includeTpl;
};

class SetToken: public iSectionToken {
public:
  static const int T_SET = 40;

public:
  SetToken(TemplateParser* parser, iBlockToken* parent): iSectionToken(parser, parent) { }

public:
  virtual int getType() {
    return T_SET;
  }
  iSectionToken::enSectionType getSectionType();
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, 
    string::iterator& itPos, bool allowCreateTokens);
  virtual string output();
  virtual void clear();
};

#endif // __TEMPLATE_TOKENS