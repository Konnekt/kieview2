/**
  *  kIEview2 Package base class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 224 $
  *  @modifiedby   $LastChangedBy: ursus6 $
  *  @lastmodified $Date: 2007-02-19 20:44:26 +0100 (Pn, 19 lut 2007) $
  */

#pragma once

#ifndef __IPACKAGE_H__
#define __IPACKAGE_H__

class iPackage : public iObject {
public:
  iPackage(const StringRef& name, const string& dir, const string& version = "", const StringRef& description = ""):
    _id(random()), _enabled(false), _name(name), _dir(dir), _version(version), _description(description) { }
  iPackage(const StringRef& name, const string& dir, bool enabled, const string& version = "", const StringRef& description = ""):
    _id(random()), _enabled(enabled), _name(name), _dir(dir), _version(version), _description(description) { }
  iPackage(): _enabled(false) { }
  virtual ~iPackage() { }

public:
  virtual UINT getID() {
    return _id;
  }

  virtual bool isEnabled() {
    return _enabled;
  }
  virtual iPackage& setEnabled(bool value) {
    _enabled = value;
    return *this;
  }

  virtual String getName() {
    return _name;
  }
  virtual iPackage& setName(const StringRef& name) {
    _name = name;
    return *this;
  }

  virtual string getVersion() {
    return _version;
  }
  virtual iPackage& setVersion(const string& version) {
    _version = version;
    return *this;
  }

  virtual String getDescription() {
    return _description;
  }
  virtual iPackage& setDescription(const StringRef& desc) {
    _description = desc;
    return *this;
  }

  virtual string getDir() {
    return _dir;
  }
  virtual iPackage& setDir(const string& dir) {
    _dir = dir;
    return *this;
  }

protected:
  UINT _id;

  String _name;
  string _version;
  String _description;

  bool _enabled;
  string _dir;
};

#endif // __IPACKAGE_H__