/**
  *  kIEview2 Zip archives manipulation class
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

#ifndef __ZIP_H__
#define __ZIP_H__

#include "unzip.h"

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

class Zip {
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
      throw CannotRead("Msg: " + getErrorMsg(result) + " in file: " + path);
    }
    return str;
  }

  bool find(const string& path) {
    int index;
    ZIPENTRY entry;

    return FindZipItem(_handle, path.c_str(), true, &index, &entry) == ZR_OK;
  }

  bool unzipItem(int index, const string& path) {
    return UnzipItem(_handle, index, path.c_str()) == ZR_OK;
  }

  bool unzip(const string& path) {
    int index = 0;

    while(unzipItem(index, path)) {
      index++;
    }
    return index;
  }

  bool setBaseDir(const string& dir) {
    return SetUnzipBaseDir(_handle, dir.c_str()) == ZR_OK;
  }

  string getErrorMsg(ZRESULT result) {
    char msg[256];
    FormatZipMessage(result, msg, 255);

    return msg;
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
      throw CannotRead("Msg: " + getErrorMsg(result) + " in file: " + path);
    }
    return str;
  }

protected:
  OFSTRUCT _ofs;
  HZIP _handle;
};

#endif // __ZIP_H__