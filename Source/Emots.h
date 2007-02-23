/**
  *  kIEview2 Emots classes
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 43 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-31 16:47:02 +0100 (N, 31 gru 2006) $
  */
  
#pragma once

#ifndef __EMOTS_H__
#define __EMOTS_H__

#include "iPackage.h"

class eM {
public:
  eM(bool in_menu = true, bool preg = true, bool __virtual = false): 
    _id(random()), _in_menu(in_menu), _virtual(__virtual), _preg(preg) { }
    virtual ~eM() { }

public:
  UINT getID() {
    return _id;
  }

  virtual ByteBuffer getRawData() {
    return _img_data;
  }
  virtual void setRawData(const ByteBuffer& new_data) {
    _img_data.assign(new_data);
  }

  virtual string getImgPath() {
    return _img_path;
  }
  virtual void setImgPath(const StringRef& new_path) {
    _img_path = new_path;
  }

  virtual string getMenuImgPath() {
    return _img_path;
  }
  virtual void setMenuImgPath(const StringRef& new_path) {
    _menu_img_path = new_path;
  }

  virtual String getText() {
    return _text;
  }
  virtual void setText(const StringRef& new_text) {
    _text = new_text;
  }

  virtual bool isVirtual() {
    return _virtual;
  }
  virtual void setVirtual(bool value) {
    _virtual = value;
  }

  virtual bool isPreg() {
    return _preg;
  }
  virtual void setPreg(bool value) {
    _preg = value;
  }

  virtual bool inMenu() {
    return _in_menu;
  }
  virtual void setInMenu(bool value) {
    _in_menu = value;
  }

protected:
  UINT _id;

  ByteBuffer _img_data;
  string _menu_img_path;
  string _img_path;
  String _text;

  bool _in_menu;
  bool _virtual;
  bool _preg;
};

class eMAuthor {
public:
  eMAuthor(const StringRef& name, const StringRef& jid = ""): _name(name), _jid(jid) { }
  eMAuthor() { }
  virtual ~eMAuthor() { }

public:
  virtual String getName() {
    return _name;
  }
  virtual void setName(const StringRef& name) {
    _name = name;
  }

  virtual String getJid() {
    return _jid;
  }
  virtual void setJid(const StringRef& jid) {
    _jid = jid;
  }

protected:
  String _name;
  String _jid;
};

class eMSet : public iPackage {
public:
  typedef list<eMAuthor> tAuthors;
  typedef vector<eM> tEmots;

public:
  eMSet(const StringRef& name, const string& version = "", const StringRef& description = ""): iPackage(name, "", version, description) { }
  eMSet(): _pos(-1) { }
  virtual ~eMSet() { }

public:
  bool operator < (eMSet& other) {
    return getPos() < other.getPos();
  }

public:
  virtual Date64 getCTime() {
    return _creationTime;
  }
  virtual void setCTime(const Date64& cTime) {
    _creationTime = cTime;
  }

  virtual String getUrl() {
    return _url;
  }
  virtual void setUrl(const StringRef& url) {
    _url = url;
  }

  virtual int getPos() {
    return _pos;
  }
  virtual void setPos(int value) {
    _pos = value;
  }

  virtual tAuthors& getAuthors() {
    return _authors;
  }
  virtual void addAuthor(const eMAuthor& author) {
    _authors.push_back(author);
  }

  virtual tEmots& getEmots() {
    return _emots;
  }
  virtual void addEmot(const eM& emot) {
    _emots.push_back(emot);
  }

protected:
  tAuthors _authors;
  tEmots _emots;

  Date64 _creationTime;
  String _url;

  int _pos;
};

class TplSet : public iPackage {
public:
  TplSet(const StringRef& name, const string& dir, const string& version = "", const StringRef& description = ""):
    iPackage(name, dir, version, description) { }
  TplSet() { }
  virtual ~TplSet() { }
};

#endif // __EMOTS_H__