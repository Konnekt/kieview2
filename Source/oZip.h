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
    Entry(const string& dir = "") {
      _data.name[0] = 0;
      setDirectory(dir);
    }
    inline Entry(const Entry& b) {
      *this = b;
    }

    inline Entry& operator = (const Entry& b) {
      memcpy(&_data, &b._data, sizeof(_data));
      this->_dir = b._dir;
      return *this;
    }

    inline ZIPENTRY* getDataRef() {
      return &_data;
    }
    inline const ZIPENTRY& operator -> () {
      return _data;
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

    inline long getFileSize() const {
      return _data.unc_size;
    }

    inline string getDirectoryName() const {
      string name = _dir.substr(0, _dir.length() - 1);
      return name.substr(name.find_last_of("\\/") + 1);
    }

    inline string getDirectory() const {
      return _dir;
    }

    inline void setDirectory(const string& dir)  {
      _dir = unifyPath(dir, true);
    }

    inline string getFileName() const {
      return _data.name;
    }

    inline string getFilePath() const {
      return _dir + this->getFileName();
    }

    inline bool isDirectory() const {
      return this->hasAttribute(FindFile::attDirectory);
    }

  protected:
    ZIPENTRY _data;
    string _dir;
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

  bool find(const string& path) {
    /*
    Entry entry(getFileDirectory(path));
    FindZipItem(_handle, path.c_str(), true, &entry.getDataRef()->index, &entry.getDataRef());
    */
    ZIPENTRY entry;
    int index;

    return !FindZipItem(_handle, path.c_str(), true, &index, &entry);
  }

  bool unzipItem(int index, const string& path) {
    return !UnzipItem(_handle, index, path.c_str());
  }

  bool unzip(const string& dir) {
    int index = 0;
    ZRESULT zr = 0;
    ZIPENTRY ze;

    while (!zr) {
      zr = GetZipItem(_handle, index, &ze);
      zr = zr || !unzipItem(index, dir + "\\" + ze.name);
      index++;
    }
    return index;
  }

  bool unzipDir(const string& dir, const string& root) {
    int index = 0;
    ZRESULT zr = 0;
    ZIPENTRY ze;
    string tmp;

    while (!zr) {
      zr = GetZipItem(_handle, index, &ze);
      tmp = ze.name;

      if (getFileDirectory(tmp).find(root) == -1) {
        index++;
        continue;
      }
      tmp = tmp.substr(root.size());
      zr = zr || !unzipItem(index, dir + "\\" + tmp);
      index++;
    }
    return index;
  }

  bool setBaseDir(const string& dir) {
    return !SetUnzipBaseDir(_handle, dir.c_str());
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

protected:
  OFSTRUCT _ofs;
  HZIP _handle;
};

#endif // __ZIP_H__