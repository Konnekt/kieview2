#pragma once

#ifndef __ITEMPLATE_TOKEN_H__
#define __ITEMPLATE_TOKEN_H__

class iBlockToken;

class iTemplateToken : public iObject {
public:
  iTemplateToken(iBlockToken* parent = NULL) : _parent(parent) { }

public:
  iBlockToken* getParent() {
    return _parent;
  }
  String getName() = 0;
  String output() = 0;

protected:
  iBlockToken* _parent;
};

#endif //__ITEMPLATE_TOKEN_H__