/**
  *  Base ListView Entry class (kIEv2 flavor)
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 ursus6
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 233 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2007-02-22 19:25:37 +0100 (Cz, 22 lut 2007) $
  */

#pragma once

#ifndef __IENTRY_H__
#define __IENTRY_H__

#include "stdafx.h"
#include "kIEview2.h"
#include "iLV.h"

using namespace Stamina::UI;
using namespace Helpers;
using namespace kIEview2;

namespace kIEview2 {
  class iEntry: public ListWnd::EntryImpl {
  public:
    STAMINA_OBJECT_CLASS_VERSION(iEntry, ListWnd::EntryImpl, Version(0,1,0,0));

  public:
    iEntry(iLV* parent, bool selected = false): _selected(selected) {
      _selectBtn = new DrawableButtonBasic(Rect(0,0,16,16), selected ? parent->_checked : parent->_unchecked);
    }
    virtual ~iEntry() { }

    virtual Size getMinSize() = 0;
    virtual Size getMaxSize() = 0;
    virtual Size getQuickSize() = 0;
    virtual Size getEntrySize(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent, Size fitIn) = 0;

    virtual void paintEntry(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent) = 0;
    virtual string getText() = 0;

    virtual void drawInfo(iLV* lv, Rect& rc);
    virtual int sizeInfo(iLV* lv, Rect& rc);

    virtual bool isSelected() {
      return _selected;
    }
    virtual void setSelected(iLV* lv, bool value = true);
    virtual void switchState(iLV* lv) {
      lv->selectItem(lv->getItemIndex(lv->getEntryItem(this)), !_selected);
    }

    virtual bool onMouseDblClk(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    virtual bool onKeyDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, int info);

  protected:
    oDrawableButton _selectBtn;
    CriticalSection _lock;

    bool _selected;
  };
}

#endif // __IENTRY_H__