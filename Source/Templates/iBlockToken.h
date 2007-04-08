#pragma once

#ifndef __IBLOCK_TOKEN_H__
#define __IBLOCK_TOKEN_H__

#include "iSectionToken.h"
#include "iVariableManager.h"

class iBlockToken: public iSectionToken, public iVariableManager {
public:
  static const int T_BLOCK = 0xFFF00000;

public:
  typedef vector<iTemplateToken*> tTokenList;

public:
  // iBlockToken(iBlockToken* token);
  iBlockToken(TemplateParser* parser, iBlockToken* parent = NULL): iSectionToken(parser, parent), iVariableManager(parent) { }
  iBlockToken(TemplateParser* parser, iVariableManager* vm): iSectionToken(parser, NULL), iVariableManager(vm) { }

public:
  virtual int getType() const {
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

  virtual String output();

protected:
  tTokenList _token;
};

#endif // __IBLOCK_TOKEN_H__