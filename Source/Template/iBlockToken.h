#pragma once

#ifndef __IBLOCK_TOKEN_H__
#define __IBLOCK_TOKEN_H__

#include "iTemplateToken.h"
#include "iVariableManager.h"

class iBlockToken: public iSectionToken, public iVariableManager {
public:
  typedef vector<iTemplateToken*> tTokenList;

public:
  iBlockToken(iBlockToken* parent = NULL): iSectionToken(parent), iVariableManager(parent) { }
  iBlockToken(iVariableManager* vm): iSectionToken(NULL), iVariableManager(vm) { }

public:
  virtual enSectionType getSectionType() {
    return tUnnamed;
  }

  virtual void add(iTemplateToken* token);
  virtual iTemplateToken* get(UINT id);
  virtual bool remove(UINT id);
  virtual UINT count();

  virtual String output();

protected:
  tTokenList _token;
};

#endif // __IBLOCK_TOKEN_H__