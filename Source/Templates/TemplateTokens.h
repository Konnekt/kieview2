#pragma once

#ifndef __TEMPLATE_TOKENS
#define __TEMPLATE_TOKENS

#include "iTemplateToken.h"
#include "iBlockToken.h"

/*
 TextToken
*/
class TextToken: public iTemplateToken {
public:
  static const int T_TEXT = 10;

public:
  TextToken(TemplateParser* parser, iBlockToken* parent): iTemplateToken(parser, parent) { }

public:
  virtual int getType() const {
    return T_TEXT;
  }
  virtual String output() {
    return _text;
  }
  virtual void clear() {
    _text.clear();
  }
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, string::iterator& itPos, bool allowCreateTokens) {
    _text = string(itCurrPos, itEnd);
    itPos = itEnd;
  }

protected:
  String _text;
};

/*
 IFToken
*/
class IFToken: public iBlockToken {
public:
  static const int T_IF = T_BLOCK | 15;

public:
  IFToken(TemplateParser* parser, iBlockToken* parent): iBlockToken(parser, parent), _ifBlock(NULL), _elseBlock(NULL) { }
  virtual ~IFToken() {
    if (_ifBlock)
      delete _ifBlock;
    if (_elseBlock)
      delete _elseBlock;
  }

public:
  virtual int getType() const {
    return T_IF;
  }
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, 
    string::iterator& itPos, bool allowCreateTokens);

  virtual void add(iTemplateToken* token);
  virtual iTemplateToken* get(UINT id);
  virtual bool remove(UINT id);
  virtual UINT count();
  virtual void clear();
  virtual String output();

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
  virtual int getType() const {
    return T_UNLESS;
  }
  iSectionToken::enSectionType getSectionType();
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, 
    string::iterator& itPos, bool allowCreateTokens);
  virtual String output();
};

class ArgumentToken: public iTemplateToken {
public:
  static const int T_ARGUMENT = 30;

public:
  ArgumentToken(TemplateParser* parser, iBlockToken* parent);
  virtual ~ArgumentToken();

public:
  virtual int getType() const {
    return T_ARGUMENT;
  }
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, 
    string::iterator& itPos, bool allowCreateTokens);
  virtual String output();
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
  virtual int getType() const {
    return T_INCLUDE;
  }
  iSectionToken::enSectionType getSectionType();
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, 
    string::iterator& itPos, bool allowCreateTokens);
  virtual String output();
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
  virtual int getType() const {
    return T_SET;
  }
  iSectionToken::enSectionType getSectionType();
  virtual void parse(iBlockToken* block, string::iterator itCurrPos, string::iterator itEnd, const string& stopToken, 
    string::iterator& itPos, bool allowCreateTokens);
  virtual String output();
  virtual void clear();
};

#endif // __TEMPLATE_TOKENS