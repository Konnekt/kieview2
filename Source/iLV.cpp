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
#include "iEntry.h"

namespace kIEview2 {
  iLV::tInstances iLV::_instances;

  iLV::iLV(sUIActionNotify_createWindow* an, int w, int h): ListWnd::ListView(an->x, an->y + 5, w, h, an->hwndParent, 0) {
    _unchecked = new Icon((HICON) ICMessage(IMI_ICONGET, kIEview2::ico::unchecked, IML_16), false);
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
    removeAll();

    DeleteObject(_fontNormal);
    DeleteObject(_fontBold);

    removeValidLV(this);
  }

  bool iLV::isValidLV(iLV* lv) {
    if (!lv) return false;

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

  ListWnd::oItem iLV::getItem(UINT id) {
    if (id >= itemsCount()) return NULL;
    int i = 0;

    for (ListWnd::ItemList::iterator it = getItemList().begin(); it != getItemList().end(); it++, i++) { 
      if (i == id) return *it;
    }
    return NULL;
  }

  void iLV::removeItem(UINT id) {
    if (id >= itemsCount()) return;

    ListWnd::oItem item = getItem(id);
    removeEntry(item->getEntry());
  }

  bool iLV::selectItem(UINT id, bool value) {
    ListWnd::oItem item = getItem(id);
    iEntry* entry;

    if (item) {
      entry = (iEntry*) item->getEntry().get();
      entry->setSelected(this, value);
      return true;
    }
    return false;
  }
}