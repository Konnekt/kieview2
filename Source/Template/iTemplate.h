#pragma once

#ifndef __ITEMPLATE_H__
#define __ITEMPLATE_H__

#include "iVariableManager.h"

class iBlockToken;

class iTemplate : public SharedObject<iSharedObject>, public iVariableManager {
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(iTemplate, iSharedObject, Version(1,0,0,0));

public:
  iTemplate(iVarManager* vm = NULL) : iVarManager(vm), _token(new iBlockToken(this)) { }
  virtual ~iTemplate() {
    delete _token;
  }

public:
  virtual iBlockToken* getToken() {
    return _token;
  }
  virtual String output() = 0;

protected:
  iBlockToken* _token;
};

typedef SharedPtr<iTemplate> oTemplate;

#endif //__ITEMPLATE_H__