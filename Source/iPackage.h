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
  virtual void setEnabled(bool value) {
    _enabled = value;
  }

  virtual String getName() {
    return _name;
  }
  virtual void setName(const StringRef& name) {
    _name = name;
  }

  virtual string getVersion() {
    return _version;
  }
  virtual void setVersion(const string& version) {
    _version = version;
  }

  virtual String getDescription() {
    return _description;
  }
  virtual void setDescription(const StringRef& desc) {
    _description = desc;
  }

  virtual string getDir() {
    return _dir;
  }
  virtual void setDir(const string& dir) {
    _dir = dir;
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