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
  Emot(string _text = "", string _imgPath = "", bool _preg = false): 
    text(_text), imgPath(_imgPath), preg(_preg) { }
  
  //TODO: findAndReplace, lub coœ w tym stylu?

public:
  string text;
  string imgPath;
  bool preg;
};

class Author {
public:
  Author(string _name = "", string _jid = ""): name(_name), jid(_jid) { }

public:
  string name;
  string jid;
};

class EmotSet {
public:
  typedef list<Emot> tEmots;
  typedef list<Author> tAuthors;

public:
  EmotSet(string _name = "", string _version = "", string _description = "", tAuthors _authors = tAuthors(), 
    string _creationTime = "", string _url = ""):
    name(_name), version(_version), description(_description), authors(_authors), creationTime(_creationTime),
    url(_url) { }
  
public:
  string name;
  string version;
  string description;
  tAuthors authors;
  string creationTime;
  string url;
  tEmots emots;
};

#endif // __EMOTS_H__