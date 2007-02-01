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

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <XZip.h>
#include <XUnzip.h>

#include <fstream>

#include "Controller.h"
#include "Emots.h"

typedef list<EmotSet> tEmotSets;

using namespace std;
using namespace Stamina;

/*
 * Base emot definition parser
 */
class EmotParserException: public ExceptionString {
public:
  EmotParserException(const StringRef& reason): ExceptionString(reason) { }
};
class WrongFormat: public EmotParserException {
public:
  WrongFormat(const StringRef& reason): EmotParserException(reason) { }
};
class CannotOpen: public EmotParserException {
public:
  CannotOpen(const StringRef& reason): EmotParserException(reason) { }
};

class EmotParser {
public:
  virtual string getDefFileName(const string& fileDir) = 0;
  virtual EmotSet parse(const string& filePath, const string& fileDir) = 0;
};

/*
 * JISP emot definition parser
 */
class JispParser: public EmotParser {
public:
  class XMLParserException: public EmotParserException {
  public:
    XMLParserException(const StringRef& reason): EmotParserException(reason) { }
  };

public:
  string getDefFileName(const string& fileDir) {
    return fileDir + ".jisp";
  }
  EmotSet parse(const string& filePath, const string& fileDir);
};

/*
 * GG emot definition parser
 */
class GGEmotParser: public EmotParser {
public:
  string getDefFileName(const string& fileDir) {
    return "emots.txt";
  }
  EmotSet parse(const string& filePath, const string& fileDir);
};

/*
 * Emots parsing class
 */
class EmotHandler : public SharedObject<iSharedObject> {
public:
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(EmotHandler, iSharedObject, Version(0,1,0,0));

public:
  typedef list<EmotParser*> tParsers;

public:
  // EmotHandler();
  ~EmotHandler() {
    for (tParsers::iterator it = parsers.begin(); it != parsers.end(); it++) {
      delete *it;
    }
  }

  string getEmotDir() {
    return Controller::getConfig()->getChar(kIEview2::cfg::emotsDir);
  }
  string getKonnektPath() {
    if (!kPath.size() && Ctrl) {
      kPath = (char*) Ctrl->ICMessage(IMC_KONNEKTDIR);
    }
    return kPath;
  }

  void addParser(EmotParser* parser) {
    parsers.push_back(parser);
  }
  String parse(cMessage* msg);

  void loadPackages();

protected:
  tEmotSets emotSets;
  tParsers parsers;
  string kPath;
};

#endif // __EMOTSHANDLER_H__