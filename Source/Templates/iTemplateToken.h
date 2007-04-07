#pragma once

#ifndef __ITEMPLATE_TOKEN_H__
#define __ITEMPLATE_TOKEN_H__

#include "TemplateParser.h"

class iBlockToken;

class iTemplateToken : public iObject {
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

#endif // __ITEMPLATE_TOKEN_H__