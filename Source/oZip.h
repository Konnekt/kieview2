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
  class Entry {
  public:
    Entry() {
      _data.name[0] = 0;
    }
    inline Entry(const Entry& b) {
      *this = b;
    }

    inline Entry& operator = (const Entry& b) {
      memcpy(&_data, &b._data, sizeof(_data));
      return *this;
    }

    inline ZIPENTRY* getDataRef() {
      return &_data;
    }
    inline ZIPENTRY* operator -> () {
      return &_data;
    }

    inline bool empty() const {
      return _data.name[0] == 0;
    }

    inline bool hasAttribute(FindFile::enAttribute attr) const {
      return (_data.attr & attr) != 0;
    }

    inline int getAttributes() const {
      return _data.attr;
    }

    inline const FILETIME& getCreationTime() const {
      return _data.ctime;
    }

    inline const FILETIME& getLastAccessTime() const {
      return _data.atime;
    }

    inline const FILETIME& getLastWriteTime() const {
      return _data.mtime;
    }

    inline int getIndex() const {
      return _data.index;
    }

    inline long getFileSize() const {
      return _data.unc_size;
    }

    inline string getDirectoryName() const {
      string name = getDirectory().substr(0, getDirectory().length() - 1);
      return name.substr(name.find_last_of("\\/") + 1);
    }

    inline string getDirectory() const {
      return getFileDirectory(getPath());
    }

    inline string getName() const {
      return ::getFileName(getPath());
    }

    inline string getPath() const {
      return _data.name;
    }

    inline bool isDirectory() const {
      return hasAttribute(FindFile::attDirectory);
    }

  protected:
    ZIPENTRY _data;
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

public:
  bool handleResult(ZRESULT resultCode) {
    if (resultCode != ZR_OK) {
      throw ExceptionString("ZIP Error [" + inttostr(resultCode) + "] { " + getErrorMsg(resultCode) + " }");
    }
    return true;
  }

  string getErrorMsg(ZRESULT result) {
    char msg[256];
    FormatZipMessage(result, msg, 255);

    return msg;
  }

  bool setBaseDir(const string& dir) {
    return handleResult(SetUnzipBaseDir(_handle, dir.c_str()));
  }

public:
  inline UINT count() {
    return get(-1)->index;
  }

  Entry find(const string& name) {
    Entry item;
    for (int i = 0; i < count(); i++) {
      item = get(i);
      if (item.getName() == name) {
        return item;
      }
    }
    throw ExceptionString("Item not found");
  }

  Entry get(const string& name) {
    Entry entry;
    handleResult(FindZipItem(_handle, name.c_str(), true, &entry->index, entry.getDataRef()));

    return entry;
  }

  Entry get(UINT index) {
    Entry entry;
    handleResult(GetZipItem(_handle, index, entry.getDataRef()));

    return entry;
  }

public:
  bool unzip(UINT index, const char* buff, UINT buffSize) {
    return handleResult(UnzipItem(_handle, index, (void*) buff, buffSize));
  }

  bool unzip(UINT index, const string& path) {
    return handleResult(UnzipItem(_handle, index, path.c_str()));
  }

  bool unzip(const string& path) {
    int index = 0;
    ZRESULT zr = 0;
    ZIPENTRY ze;

    while (!zr) {
      zr = GetZipItem(_handle, index, &ze);
      zr = zr || !unzip(index, path + "\\" + ze.name);
      index++;
    }
    return index;
  }

  bool unzipDir(const string& path, const string& root) {
    int index = 0;
    ZRESULT zr = 0;
    ZIPENTRY ze;
    string tmp;

    while (!zr) {
      zr = GetZipItem(_handle, index, &ze);
      tmp = ze.name;

      int pos = getFileDirectory(tmp).find(root);
      if (pos == -1 || pos > 0) {
        index++;
        continue;
      }
      tmp = tmp.substr(root.size());
      zr = zr || !unzip(index, path + "\\" + tmp);
      index++;
    }
    return index;
  }

public:
  String getFile(const string& path) {
    Entry file = find(path);

    if (file.empty()) {
      throw NotFound("Cannot find file " + path);
    }

    String str;
    try {
      char* buff = new char[file.getFileSize() + 1];
      buff[file.getFileSize()] = '\0';

      unzip(file.getIndex(), buff, file.getFileSize());
      str.assign(buff);
      delete [] buff;
    } catch(...) {
      throw;
    }
    return str;
  }

  ByteBuffer getBinaryFile(const string& path) {
    Entry file = find(path);

    if (file.empty()) {
      throw NotFound("Cannot find file " + path);
    }

    ByteBuffer str;
    try {
      char* buff = new char[file.getFileSize() + 1];
      buff[file.getFileSize()] = '\0';

      unzip(file.getIndex(), buff, file.getFileSize());
      str.assign((const unsigned char*) buff, file.getFileSize() + 1);
      delete [] buff;
    } catch(...) {
      throw;
    }
    return str;
  }

protected:
  OFSTRUCT _ofs;
  HZIP _handle;
};

#endif // __ZIP_H__