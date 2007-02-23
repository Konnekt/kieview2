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
  eM(bool in_menu = true, bool preg = true): _id(random()), _in_menu(in_menu), _preg(preg) { }
  virtual ~eM() { }

public:
  inline UINT getID() const {
    return _id;
  }

  virtual string getImgPath() const {
    return _img_path;
  }
  virtual void setImgPath(const StringRef& new_path) {
    _img_path = new_path;
  }

  virtual string getMenuImgPath() const {
    return _img_path;
  }
  virtual void setMenuImgPath(const StringRef& new_path) {
    _menu_img_path = new_path;
  }

  virtual String getText() const {
    return _text;
  }
  virtual void setText(const StringRef& new_text) {
    _text = new_text;
  }

  virtual bool isPreg() const {
    return _preg;
  }
  virtual void setPreg(bool value) {
    _preg = value;
  }

  virtual bool inMenu() const {
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
  bool _preg;
};

class eMAuthor {
public:
  eMAuthor(const StringRef& name, const StringRef& jid = ""): _name(name), _jid(jid) { }
  eMAuthor() { }
  virtual ~eMAuthor() { }

public:
  virtual String getName() const {
    return _name;
  }
  virtual void setName(const StringRef& name) {
    _name = name;
  }

  virtual String getJid() const {
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
  bool operator < (eMSet& other) const {
    return getPos() < other.getPos();
  }

public:
  virtual const Date64& getCTime() const {
    return _creationTime;
  }
  virtual void setCTime(const Date64& cTime) {
    _creationTime = cTime;
  }

  virtual String getUrl() const {
    return _url;
  }
  virtual void setUrl(const StringRef& url) {
    _url = url;
  }

  virtual int getPos() const {
    return _pos;
  }
  virtual void setPos(int value) {
    _pos = value;
  }

  virtual const tAuthors& getAuthors() const {
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

#endif // __EMOTS_H__