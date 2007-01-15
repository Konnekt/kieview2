/**
  *  kIEview2 Emots classes
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @copyright    Copyright (c) 2006-2007 Micha³ "Dulek" Dulko
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 43 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-31 16:47:02 +0100 (N, 31 gru 2006) $
  */
  
#pragma once

#ifndef __EMOTS_H__
#define __EMOTS_H__

class Emot {
public:
  Emot(const StringRef& _text = "", const string& _imgPath = "", bool _preg = false): 
    text(_text), imgPath(_imgPath), preg(_preg) { }
  
  // TODO: findAndReplace, lub coœ w tym stylu?

public:
  String text;
  string imgPath;
  bool preg;
};

class EmotAuthor {
public:
  EmotAuthor(const StringRef& _name = "", const StringRef& _jid = ""): name(_name), jid(_jid) { }

public:
  String name;
  String jid;
};

class EmotSet {
public:
  typedef list<EmotAuthor> tAuthors;
  typedef list<Emot> tEmots;

public:
  EmotSet(const StringRef& _name = "", const string& _version = "", const StringRef& _description = "", 
    const tAuthors& _authors = tAuthors(), const string& _creationTime = "", const StringRef& _url = ""):
    name(_name), version(_version), description(_description), authors(_authors), creationTime(_creationTime),
    url(_url) { }
  
public:
  tAuthors authors;
  tEmots emots;

  String name;
  string version;
  String description;
  string creationTime;
  String url;
};

#endif // __EMOTS_H__