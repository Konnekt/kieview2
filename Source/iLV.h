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

#pragma once

#ifndef __ILV_H__
#define __ILV_H__

#include "stdafx.h"
#include "kIEview2.h"
#include "Helpers.h"

using namespace Stamina::UI;
using namespace Helpers;
using namespace kIEview2;

namespace kIEview2 {
  class iLV: public ListWnd::ListView {
  public:
    STAMINA_OBJECT_CLASS_VERSION(iLV, ListWnd::ListView, Version(0,1,0,0));

  public:
    typedef vector<iLV*> tInstances;

  public:;
    iLV(sUIActionNotify_createWindow* an, int w, int h);
    virtual ~iLV();

    static bool isValidLV(iLV* lv);
    static void addValidLV(iLV* lv);
    static void removeValidLV(iLV* lv);

    virtual ListWnd::oItem getItem(UINT id) {
      if (id >= itemsCount()) return NULL;
      int i = 0;

      for (ListWnd::ItemList::iterator it = getItemList().begin(); it != getItemList().end(); it++, i++) { 
        if (i == id) return *it;
      }
      return NULL;
    }
    virtual UINT itemsCount() {
      return getItemList().count();
    }
    virtual void removeItem(UINT id) {
      if (id >= itemsCount()) return;

      ListWnd::oItem item = getItem(id);
      removeEntry(item->getEntry());
    }
    virtual bool selectItem(UINT id, bool value = true) {
      ListWnd::oItem item = getItem(id);
      iEntry* entry;

      if (item) {
        entry = (iEntry*) item->getEntry().get();
        entry->setSelected(this, value);
        return true;
      }
      return false;
    }

    virtual void saveState() = 0;

    virtual void setSize(int w, int h);
    virtual void setPos(int x, int y);

  protected:
    static tInstances _instances;

    oImage _unChecked;
    oImage _checked;

    HFONT _fontNormal;
    HFONT _fontBold;

  public:
    class iEntry: public ListWnd::EntryImpl {
    public:
      STAMINA_OBJECT_CLASS_VERSION(iEntry, ListWnd::EntryImpl, Version(0,1,0,0));

      iEntry(iLV* parent, bool selected = false): _selected(selected) {
        _selectBtn = new DrawableButtonBasic(Rect(0,0,16,16), selected ? parent->_checked : parent->_unChecked);
      }

      virtual Size getMinSize() = 0;
      virtual Size getMaxSize() = 0;
      virtual Size getQuickSize() = 0;
      virtual Size getEntrySize(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent, Size fitIn) = 0;

      virtual void paintEntry(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent) = 0;
      virtual string getText() = 0;

      virtual void drawInfo(iLV* lv, Rect& rc) {
        string text = getText();
        HDC dc = lv->getDC();
        RECT rcz = *rc.ref();

        HFONT oldFont = (HFONT) SelectObject(dc, lv->_fontNormal);
        DrawText(dc, text.c_str(), -1, &rcz, DT_WORDBREAK);
        SelectObject(dc, oldFont);
        lv->releaseDC(dc);
      }
      virtual int sizeInfo(iLV* lv, Rect& rc) {
        string text = getText();
        HDC dc = lv->getDC();
        RECT rcz = {0, 0, rc.width(), 0};

        HFONT oldFont = (HFONT) SelectObject(dc, lv->_fontNormal);
        DrawText(dc, text.c_str(), -1, &rcz, DT_CALCRECT | DT_WORDBREAK);
        SelectObject(dc, oldFont);
        lv->releaseDC(dc);

        return rcz.bottom;
      }

      virtual bool isSelected() {
        return _selected;
      }
      virtual void setSelected(iLV* lv, bool value = true) {
        _selectBtn->setImage(value ? lv->_unChecked.get() : lv->_checked.get());
        _selected = value;
        refreshEntry(lv, ListWnd::RefreshFlags::refreshPaint);
        touchConfigWnd();
      }
      virtual void switchState(iLV* lv) {
        setSelected(lv, !_selected);
      }

      virtual bool onMouseDblClk(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
        LockerCS locker(_lock);

        if (vkey == VK_LBUTTON) {
          switchState((iLV*) lv);
          return false;
        }
        return true;
      }
      virtual bool onKeyDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, int info) {
        LockerCS locker(_lock);

        if (vkey == VK_SPACE) {
          switchState((iLV*) lv);
          return false;
        }
        return true;
      }

    protected:
      oDrawableButton _selectBtn;
      CriticalSection _lock;

      bool _selected;
    };
  };
}

#endif // __ILV_H__