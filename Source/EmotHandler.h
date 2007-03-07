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

public:
  EmotHandler& operator >> (StringRef& body) {
    body = parse(body);
    return *this;
  }

public:
  string getDir() {
    return __super::getDir(kIEview2::cfg::emotsDir);
  }
  String parse(const StringRef& body);

  void fillLV(iLV* lv);

  void loadSettings();
  void saveSettings();

protected:
  String prepareBody(const StringRef& body, bool encode = true, bool html = true);
  void parseSet(RegEx& reg, eMSet& set);

  static string __stdcall emotInsertion(RegEx* reg, void* param);
  static string __stdcall replaceEmot(RegEx* reg, void* param);

protected:
  tEmotInsertions emotInsertions;
};

#endif // __EMOTSHANDLER_H__