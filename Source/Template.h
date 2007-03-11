#pragma once

#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

class Template {
public:
  Template(const StringRef& path);
  ~Template();

private:
  iTemplateToken _token;

protected:
};

class iTemplateToken {
public:
  typedef vector<iTemplateToken*> tTokens;

public:
  iTemplateToken(iTemplateToken* parent = NULL): _parent(parent) { }

protected:
  virtual void set(StringRef& text) = 0;
  virtual void parse() = 0;
  virtual String output() = 0;

protected:
  virtual UINT tokenAdd(const StringRef& text) = 0;
  virtual iToken* tokenGet(UINT id) = 0; 
  virtual void tokenRemove(UINT id) = 0;
  virtual UINT tokenCount() = 0;

protected:
  iTemplateToken* _parent;
  tTokens _tokens;
  String _text;

  //tLocals _


}; #endif