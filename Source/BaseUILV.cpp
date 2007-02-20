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

#include "stdafx.h"
#include "BaseUILV.h"

BaseUILV::tBaseLVs BaseUILV::_lvs;

BaseUILV::BaseUILV(int x, int y, int w, int h, HWND parent, HMENU id): ListWnd::ListView(x, y, w, h, parent, id) {
  alwaysShowScrollbars(false, false);

  addValidLV(this);
}

BaseUILV::~BaseUILV() {
  removeValidLV(this);
}

bool BaseUILV::isValidLV(BaseUILV* lv) {
  for (tBaseLVs::iterator it = _lvs.begin(); it != _lvs.end(); it++) {
    if ((*it) == lv) return true;
  }
  return false;
}

void BaseUILV::addValidLV(BaseUILV* lv) {
  _lvs.push_back(this);
}

void BaseUILV::removeValidLV(BaseUILV* lv) {
  for (tBaseLVs::iterator it = _lvs.begin(); it != _lvs.end(); it++) {
    if ((*it) == this) {
      _lvs.erase(it); break;
    }
  }
}

void BaseUILV::setPos(int x, int y) {
  SetWindowPos(getHwnd(), 0, x, y, 0, 0, SWP_NOSIZE);
}

void BaseUILV::setSize(int w, int h) {
  SetWindowPos(getHwnd(), 0, 0, 0, w, h, SWP_NOMOVE);
}
