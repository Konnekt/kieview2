/**
  *  kIEview2 Emot handler class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 19 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-15 17:31:48 +0100 (Pt, 15 gru 2006) $
  */

#include "stdafx.h"
#include "Helpers.h"

#include "EmotHandler.h"
#include "Controller.h"

void EmotHandler::loadSettings() {
  string data = Controller::getConfig()->getChar(cfg::emotPacks);
  data = trim(data, "\n");

  if (!data.length()) return;

  tStringVector sets;
  Stamina::split(data, "\n", sets);

  for (tStringVector::iterator it = sets.begin(); it != sets.end(); it++) {
    tStringVector rows;
    Stamina::split(*it, "|", rows);

    if (rows.size() != 3) continue;
    for (tEmotSets::iterator it2 = emotSets.begin(); it2 != emotSets.end(); it2++) {
      if (it2->getDir() == rows[2]) {
        it2->setEnabled(atoi(rows[1].c_str()));
        it2->setPos(atoi(rows[0].c_str()));
        break;
      }
    }
  }
  emotSets.sort();
}

void EmotHandler::saveSettings() {
  if (!emotSets.size()) return;
  String result;

  for (tEmotSets::iterator it = emotSets.begin(); it != emotSets.end(); it++) {
    result += stringf("%d|%d|%s\n", it->getPos(), (int) it->isEnabled(), it->getDir().c_str());
  }
  Controller::getConfig()->set(cfg::emotPacks, result);
}

void EmotHandler::fillLV(iLV* _lv) {
  EmotLV* lv = (EmotLV*) _lv;
  oImage img;

  for (tEmotSets::iterator it = emotSets.begin(); it != emotSets.end(); it++) {
    if (!it->getEmots()[0].isVirtual()) {
      img = loadImageFromFile((getDir() + "\\" + it->getDir() + "\\" + it->getEmots()[0].getMenuImgPath()).c_str());
    } else {
      img = new Icon((HICON) Ctrl->ICMessage(IMI_ICONGET, kIEview2::ico::emots, IML_16), false);
    }
    lv->addItem(new EmotLV::sEmotPackInfo(it->isEnabled(), &*it, img));
  }
}

String EmotHandler::prepareBody(const StringRef& body, bool encode, bool html) {
  RegEx reg;
  reg.setSubject(body);

  if (encode) {
    reg.replaceItself(html ? "/&amp;/" : "/&/", "\1");
    reg.replaceItself(html ? "/&lt;/" : "/</", "\2");
    reg.replaceItself(html ? "/&gt;/" : "/>/", "\3");
    reg.replaceItself(html ? "/&quot;/" : "/\"/", "\4");
    reg.replaceItself(html ? "/&(apos|#0?39);/" : "/'/", "\5");
  } else {
    reg.replaceItself("/\1/", html ? "&amp;" : "/&/");
    reg.replaceItself("/\2/", html ? "&lt;" : "/</");
    reg.replaceItself("/\3/", html ? "&gt;" : "/>/");
    reg.replaceItself("/\4/", html ? "&quot;" : "/\"/");
    reg.replaceItself("/\5/", html ? "&#039;" : "/'/");
  }
  return reg.getSubject();
}

string __stdcall EmotHandler::emotInsertion(RegEx* reg, void* param) {
  sEmotInsertion* ei = static_cast<sEmotInsertion*>(param);
  ei->match = reg->getSub(0).c_str();

  return "<kiev2:emot:insertion id=\"" + inttostr(ei->id) + "\" />";
}

string __stdcall EmotHandler::replaceEmot(RegEx* reg, void* param) {
  EmotHandler* handler = static_cast<EmotHandler*>(param);
  sEmotInsertion* ei = &handler->emotInsertions.at(atoi(reg->getSub(1).c_str()));

  IMLOG("[EmotHandler::replaceEmot()] match = %s, img_path = %s, emot = %s, set = %s", ei->match.c_str(), 
    ei->emot->getImgPath().c_str(), ei->emot->getText().c_str(), ei->emotSet->getName().c_str());

  return "<img src=\"" + 
    (ei->emot->isVirtual() ? 
      "javascript:oController.getEmot(" + inttostr(ei->emot->getID()) + ");" :
      unifyPath(handler->getDir() + "/" + ei->emotSet->getDir() + "/" + ei->emot->getImgPath())
    ) + "\" class=\"emoticon\" alt=\"" + ei->match + "\" />";
}

void EmotHandler::parseSet(RegEx& reg, eMSet& set) {
  if (!set.isEnabled()) return;

  for (eMSet::tEmots::iterator it = set.getEmots().begin(); it != set.getEmots().end(); it++) {
    sEmotInsertion ei(emotInsertions.size(), &*it, &set);
    try {
      reg.setPattern(prepareBody(!it->isPreg() ? "/" + reg.addSlashes(it->getText()) + "/i" : it->getText(), true, false));
      reg.replaceItself(&EmotHandler::emotInsertion, 0, (void*) &ei);
    } catch (const RegEx::CompileException& e) {
      IMLOG("[EmotHandler::parseSet()] B³¹d definicji emotikony: %s, pos %i", e.error, e.pos);
      continue;
    }
    emotInsertions.push_back(ei);
  }
}

String EmotHandler::parse(const StringRef& body) {
  RegEx reg;
  reg.setSubject(prepareBody(body));

  for (tEmotSets::iterator it = emotSets.begin(); it != emotSets.end(); it++) {
    parseSet(reg, *it);
  }

  reg.replaceItself("#<kiev2:emot:insertion id=\"([0-9]+)\" />#", &EmotHandler::replaceEmot, 0, (void*) this);
  emotInsertions.clear();

  return prepareBody(reg.getSubject(), false);
}

void EmotHandler::loadPackages() {
  clearPackages();

  FindFile find;
  find.setMask(getDir() + "\\*");
  find.setDirOnly();

  FindFile::tFoundFiles emotDirs = find.makeList();

  if (find.nothingFound()) {
    IMLOG("[EmotHandler::loadPackages()] Nie znaleziono katalogu z emotami !");
    return;
  }
  if (!emotDirs.size()) {
    IMLOG("[EmotHandler::loadPackages()] Brak katalogów z pakietami !");
    return;
  }

  for (FindFile::tFoundFiles::iterator it = emotDirs.begin(); it != emotDirs.end(); it++) {
    for (tParsers::iterator it2 = parsers.begin(); it2 != parsers.end(); it2++) {

      string fileName = unifyPath(it->getDirectory() + "/" + (*it2)->getDefFileName(it->getFileName()));
      if (!fileExists(fileName.c_str())) continue;

      try {
        emotSets.push_back((*it2)->parse(fileName, it->getFileName()));
      } catch (const Exception& e) {
        IMLOG("[EmotHandler::loadPackages()] b³¹d podczas parsowania paczki emot (%s): %s", it->getFileName(), e.getReason().a_str());
      }
      break;
    }
  }
}