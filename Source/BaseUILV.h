/**
  *  kIEview2
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 ursus6
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision$
  *  @modifiedby   $LastChangedBy$
  *  @lastmodified $Date$
  */

#pragma once

#ifndef __BASELV_UI_H__
#define __BASELV_UI_H__

#include "stdafx.h"
#include "kIEview2.h"

class BaseUILV: public ListWnd::ListView {
public:
  STAMINA_OBJECT_CLASS_VERSION(BaseUILV, ListWnd::ListView, Version(0,1,0,0));

public:
  typedef vector<BaseUILV*> tBaseLVs;

public:;
  BaseUILV(int x, int y, int w, int h, HWND parent, HMENU id);
  virtual ~BaseUILV();

  static bool isValidLV(BaseUILV* lv);
  void addValidLV(BaseUILV* lv);
  void removeValidLV(BaseUILV* lv);

  virtual void setPos(int x, int y);
  virtual void setSize(int w, int h);

protected:
  static tBaseLVs _lvs;
};

#endif //__BASELV_UI_H__