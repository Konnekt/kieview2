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

#ifndef __STYLE_UI_H__
#define __STYLE_UI_H__

#include "stdafx.h"
#include "kIEview2.h"

#include "iLV.h"
#include "Emots.h"
#include "Helpers.h"

using namespace Stamina::UI;
using namespace Helpers;
using namespace kIEview2;

class StyleLV: public iLV {
public:
  STAMINA_OBJECT_CLASS_VERSION(StyleLV, iLV, Version(0,1,0,0));

public:
  StyleLV(sUIActionNotify_createWindow* an, int w, int h);
  virtual ~StyleLV();

public:
  struct sStylePackInfo {
    TplSet* set;
    bool used;
    UINT id;

    sStylePackInfo(bool used, TplSet* _set): used(used), set(_set), id(-1) { }
    sStylePackInfo(): used(false) { }
  };

  typedef std::vector<ListWnd::Item*> tItems;

public:

  virtual UINT addItem(sStylePackInfo* pack);
  virtual void removeItem(UINT id);
  virtual void removeAllItems();
  virtual int itemsCount();
  virtual ListWnd::oItem getItem(UINT id);
  virtual sStylePackInfo* getSPI(UINT id);
  virtual void selectRadio(UINT id);

  virtual void saveState() {
    if (!itemsCount()) return;

    for (UINT i = 0; i < itemsCount(); i++) {
      sStylePackInfo* info = getSPI(i);
      info->set->setEnabled(info->used);
    }
  }

protected:
  oImage _checked;
  oImage _unchecked;
  UINT _last_checked;

  HFONT hFontBold;
  HFONT hFont;

public:
  class StyleInfoItem: public ListWnd::EntryImpl {
  public:
    friend StyleLV;

  public:
    STAMINA_OBJECT_CLASS_VERSION(StyleInfoItem, ListWnd::EntryImpl, Version(0,1,0,0));

    StyleInfoItem(StyleLV* parent, sStylePackInfo* styleInfo): _styleInfo(styleInfo) {
      _radio = new DrawableButtonBasic(Rect(0,0,16,16), styleInfo->used ? parent->_checked : parent->_unchecked);
    }

    Size getMinSize();
    Size getMaxSize();
    Size getQuickSize();
    Size getEntrySize(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent, Size fitIn);

    void paintEntry(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent);
    void drawInfo(StyleLV* slv, Rect& rc);
    int sizeInfo(StyleLV* slv, Rect& rc);
    void resizeItems(StyleLV* slv, ListWnd::Item* item);

    string getItemText() {
      TplSet* set = _styleInfo->set;
      string text;

      if (set->getDescription().length()) text += "Opis: " + set->getDescription();
      if (text.length()) text = rtrim(text.c_str(), "\n");

      return text;
    }

    bool onMouseDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    bool onMouseDblClk(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    bool onKeyDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, int info);

    sStylePackInfo* getStylePackInfo();

  protected:
    oDrawableButton _radio;
    sStylePackInfo* _styleInfo;
    CriticalSection _lock;
  };
};

#endif  // __STYLE_UI_H__