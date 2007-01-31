/**
  *  kIEview2 Emot handler class
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

#ifndef __EMOTSHANDLER_H__
#define __EMOTSHANDLER_H__

#include "Emots.h"

class EmotHandler : public SharedObject<iSharedObject> {
public:
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(EmotHandler, iSharedObject, Version(0,1,0,0));

public:
  typedef list<EmotSet> tEmotSets;
  typedef list<string> tEmotDirs;

  class EmotParser {
  public:
    class WrongFormat: public ExceptionString {
    public:
      WrongFormat(const StringRef& reason): ExceptionString(reason) { }
    };

  public:
    virtual EmotSet parse(string str) = 0;
  };

  class JispParser: public EmotParser {
  public:
    class XMLParserException: public ExceptionString {
    public:
      XMLParserException(const StringRef& reason): ExceptionString(reason) { }
    };

  public:
    JispParser() {
      this->parser.set_substitute_entities();
    }

  public:
    EmotSet parse(string str);

  protected:
    xmlpp::DomParser parser;
  };

  class GGEmotParser: public EmotParser {
  public:
    EmotSet parse(string str);

  protected:
    RegEx parser;
  };

  class EmotReplacer {
  public:
    String replace(StringRef& str, EmotSet& set);

  protected:
    RegEx parser;
  };

public:
  EmotHandler(const string& emotDir = "emots");

  inline void setKonnektPath(const string& path) {
    kPath = path;
  }
  inline void setEmotDir(const string& dir) {
    emotDir = dir;
  }

  void addEmotDir(const string& dir);
  
  void loadPackages();

protected:
  tEmotSets emotSets;
  tEmotDirs emotDirs;
  string kPath;
  string emotDir;
  GGEmotParser ggEmotParser;
  JispParser jispParser;
  
};

#endif // __EMOTSHANDLER_H__