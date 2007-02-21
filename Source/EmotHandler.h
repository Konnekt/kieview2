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

#include "unzip.h"
#include <fstream>

// #include "Base64.h"
#include "Helpers.h"
#include "EmotUI.h"
#include "Emots.h"
#include "StyleUI.h"

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

class eMParser {
public:
  virtual string getDefFileName(const string& fileDir) = 0;
  virtual eMSet parse(const string& filePath, const string& fileDir) = 0;
};

class Dir {
public:
  class NotFound: public ExceptionString {
  public:
    NotFound(const StringRef& reason): ExceptionString(reason) { }
  };

public:
  typedef list<WIN32_FIND_DATA> tItems;

public:
  static bool hasItem(const string& name, tItems items) {
    for (tItems::iterator it = items.begin(); it != items.end(); it++) {
      if (name == it->cFileName) return true;
    }
    return false;
  }

  static bool isFile(WIN32_FIND_DATA fd) {
    return !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
  }

  static bool isDir(WIN32_FIND_DATA fd) {
    return fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
  }

  static bool isDot(WIN32_FIND_DATA fd) {
    return fd.cFileName == "." || fd.cFileName == "..";
  }

  static tItems getItems(const string& dir) {
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA fd;

    if ((hFind = FindFirstFile(dir.c_str(), &fd)) == INVALID_HANDLE_VALUE) {
      throw NotFound(inttostr(GetLastError()));
    }

    tItems items;
    do {
      if (!isDot(fd)) items.push_back(fd);
    }
    while (FindNextFile(hFind, &fd));

    FindClose(hFind);
    return items;
  }

  static tItems getFiles(const string& dir) {
    tItems items = getItems(dir);

    for (tItems::iterator it = items.begin(); it != items.end();) {
      if (!isFile(*it)) it = items.erase(it);
      else it++;
    }
    return items;
  }

  static tItems getDirs(const string& dir) {
    tItems items = getItems(dir);

    for (tItems::iterator it = items.begin(); it != items.end();) {
      if (!isDir(*it)) it = items.erase(it);
      else it++;
    }
    return items;
  }
};

class Zip {
public:
  class NotFound: public ExceptionString {
  public:
    NotFound(const StringRef& reason): ExceptionString(reason) { }
  };

  class CannotOpen: public ExceptionString {
  public:
    CannotOpen(const StringRef& reason): ExceptionString(reason) { }
  };

  class CannotRead: public ExceptionString {
  public:
    CannotRead(const StringRef& reason): ExceptionString(reason) { }
  };

public:
  Zip(const string& path): _handle(NULL) {
    open(path);
  }
  Zip(): _handle(NULL) { }

  ~Zip() {
    close();
  }

public:
  void open(const string& path) {
    if (_handle) close();

    if (!(_handle = OpenZip(path.c_str(), 0))) {
      throw CannotOpen("Cannot open file " + path);
    }
  }
  void close() {
    if (!_handle) return;

    CloseZip(_handle);
    _handle = NULL;
  }

  ByteBuffer getBinaryFile(const string& path) {
    ZIPENTRY entry;
    int index;

    if (FindZipItem(_handle, path.c_str(), true, &index, &entry) != ZR_OK) {
      throw NotFound("Cannot find file " + path);
    }

    char* buff = new char[entry.unc_size + 1];
    buff[entry.unc_size] = '\0';
    ByteBuffer str;

    ZRESULT result = UnzipItem(_handle, index, buff, entry.unc_size + 1);
    str.assign((const unsigned char*) buff, entry.unc_size + 1);
    delete [] buff;

    if (result != ZR_OK) {
      char errMsg[256];
      FormatZipMessage(result, errMsg, 255);

      throw CannotRead("Msg: " + string(errMsg) + " in file: " + path);
    }
    return str;
  }

  String getFile(const string& path) {
    ZIPENTRY entry;
    int index;

    if (FindZipItem(_handle, path.c_str(), true, &index, &entry) != ZR_OK) {
      throw NotFound("Cannot find file " + path);
    }

    char* buff = new char[entry.unc_size + 1];
    buff[entry.unc_size] = '\0';
    String str;

    ZRESULT result = UnzipItem(_handle, index, buff, entry.unc_size);
    str.assign(buff);
    delete [] buff;

    if (result != ZR_OK) {
      char errMsg[256];
      FormatZipMessage(result, errMsg, 255);

      throw CannotRead("Msg: " + string(errMsg) + " in file: " + path);
    }
    return str;
  }

protected:
  OFSTRUCT _ofs;
  HZIP _handle;
};

/*
 * JISP emot definition parser
 */
class JispParser: public eMParser {
public:
  class XMLParserException: public EmotParserException {
  public:
    XMLParserException(const StringRef& reason): EmotParserException(reason) { }
  };

public:
  string getDefFileName(const string& fileDir) {
    return fileDir + ".jisp";
  }
  eMSet parse(const string& filePath, const string& fileDir);
};

/*
 * GG emot definition parser
 */
class GGParser: public eMParser {
public:
  string getDefFileName(const string& fileDir) {
    return "emots.txt";
  }
  eMSet parse(const string& filePath, const string& fileDir);
};

/*
 * Style parsing class
 */
class StyleHandler : public SharedObject<iSharedObject> {
public:
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(StyleHandler, iSharedObject, Version(0,1,0,0));

public:
  typedef list<TplSet> tStyleSets;

public:
  void fillLV(StyleLV* lv);

  void clearPackages() {
    if (styleSets.size()) styleSets.clear();
  }

  void reloadPackages(StyleLV* lv = 0) {
    if (!StyleLV::isVaildLV(lv)) lv = 0;

    if (lv) lv->removeAllItems();
    loadPackages();
    loadSettings();
    if (lv) fillLV(lv);
  }
  void loadPackages();

  void loadSettings();
  void saveSettings();

protected:
  tStyleSets styleSets;
};

/*
 * Emots parsing class
 */
class EmotHandler : public SharedObject<iSharedObject> {
public:
  /* Class version */
  STAMINA_OBJECT_CLASS_VERSION(EmotHandler, iSharedObject, Version(0,1,0,0));

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
  // EmotHandler();
  ~EmotHandler() {
    for (tParsers::iterator it = parsers.begin(); it != parsers.end(); it++) {
      delete *it;
    }
  }

  string getKonnektPath();
  string getEmotDir();

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

  void fillLV(EmotLV* lv);

  void clearPackages() {
    if (emotSets.size()) emotSets.clear();
  }

  void reloadPackages(EmotLV* lv = 0) {
    if (!EmotLV::isValidLV(lv)) lv = 0;

    if (lv) lv->removeAllItems();
    loadPackages();
    loadSettings();
    if (lv) fillLV(lv);
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
