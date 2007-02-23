/**
  *  kIEview2 Emot handler class
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

#ifndef __EMOTSHANDLER_H__
#define __EMOTSHANDLER_H__

#include <stamina/findfile.h>

#include "Helpers.h"
#include "iPackageHandler.h"
#include "EmotParser.h"
#include "Emots.h"

class EmotHandler : public iPackageHandler {
public:
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(EmotHandler, iPackageHandler, Version(0,1,0,0));

public:
  struct sEmotInsertion {
    string match;
    UINT id;

    eMSet* emotSet;
    eM* emot;

    sEmotInsertion(UINT _id, eM* _emot, eMSet* _emotSet): id(_id), emot(_emot), emotSet(_emotSet) { }
    sEmotInsertion(): id(0), emotSet(0), emot(0) { }
  };

public:
  typedef vector<sEmotInsertion> tEmotInsertions;
  typedef map<int, list<eMSet*>> tNetEmotSets;
  typedef list<eMParser*> tParsers;
  typedef list<eMSet> tEmotSets;

public:
  EmotHandler() {
    _dirColID = kIEview2::cfg::emotsDir;
  }
  ~EmotHandler() {
    for (tParsers::iterator it = parsers.begin(); it != parsers.end(); it++) {
      delete *it;
    }
  }

  eMSet* getEmotSet(UINT id) {
    for (tEmotSets::iterator it = emotSets.begin(); it != emotSets.end(); it++) {
      if (it->getID() == id) return &*it;
    }
    throw ExceptionString("EmotSet not found");
  }
  eM* getEmot(UINT id) {
    for (tEmotSets::iterator it = emotSets.begin(); it != emotSets.end(); it++) {
      for (eMSet::tEmots::iterator it2 = it->getEmots().begin(); it2 != it->getEmots().end(); it2++) {
        if (it2->getID() == id) return &*it2;
      }
    }
    throw ExceptionString("Emot not found");
  }

  void addParser(eMParser* parser) {
    parsers.push_back(parser);
  }
  String parse(const StringRef& body, int net);

  void fillLV(iLV* lv);

  void clearPackages() {
    if (emotSets.size()) emotSets.clear();
  }
  void loadPackages();

  void loadSettings();
  void saveSettings();

protected:
  String prepareBody(const StringRef& body, bool encode = true, bool html = true);
  void parseSet(RegEx& reg, eMSet& set);

  static string __stdcall emotInsertion(RegEx* reg, void* param);
  static string __stdcall replaceEmot(RegEx* reg, void* param);

protected:
  tEmotInsertions emotInsertions;
  tEmotSets emotSets;
  tParsers parsers;
};

#endif // __EMOTSHANDLER_H__