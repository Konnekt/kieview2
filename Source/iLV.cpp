/**
  *  Base ListView class (kIEv2 flavor)
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
#include "iLV.h"

namespace kIEview2 {
  iLV::tInstances iLV::_instances;

  iLV::iLV(sUIActionNotify_createWindow* an, int w, int h): ListWnd::ListView(an->x, an->y + 5, w, h, an->hwndParent, 0) {
    _unChecked = new Icon((HICON) ICMessage(IMI_ICONGET, kIEview2::ico::unchecked, IML_16), false);
    _checked = new Icon((HICON) ICMessage(IMI_ICONGET, kIEview2::ico::checked, IML_16), false);

    _fontNormal = CreateFont(-11, 0, 0, 0, FW_MEDIUM, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
      CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");
    _fontBold = CreateFont(-11, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
      CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");

    alwaysShowScrollbars(false, false);
    addValidLV(this);

    an->x += w;
    an->y += h + 5;

    an->w += w;
    an->h += h + 5;

    an->hwnd = getHwnd();
  }

  iLV::~iLV() {
    DeleteObject(_fontNormal);
    DeleteObject(_fontBold);

    removeValidLV(this);
  }

  bool iLV::isValidLV(iLV* lv) {
    for (tInstances::iterator it = _instances.begin(); it != _instances.end(); it++) {
      if (*it == lv) return true;
    }
    return false;
  }

  void iLV::addValidLV(iLV* lv) {
    _instances.push_back(lv);
  }

  void iLV::removeValidLV(iLV* lv) {
    for (tInstances::iterator it = _instances.begin(); it != _instances.end(); it++) {
      if (*it == lv) {
        _instances.erase(it); break;
      }
    }
  }

  void iLV::setSize(int w, int h) {
    SetWindowPos(getHwnd(), 0, 0, 0, w, h, SWP_NOMOVE);
  }

  void iLV::setPos(int x, int y) {
    SetWindowPos(getHwnd(), 0, x, y, 0, 0, SWP_NOSIZE);
  }
}