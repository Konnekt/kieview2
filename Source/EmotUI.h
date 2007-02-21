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

#ifndef __EMOT_UI_H__
#define __EMOT_UI_H__

#include "stdafx.h"
#include "kIEview2.h"

#include "iLV.h"

#include "Helpers.h"
#include "Emots.h"

using namespace Stamina::UI;
using namespace Helpers;
using namespace kIEview2;

class EmotLV: public iLV {
public:
  STAMINA_OBJECT_CLASS_VERSION(EmotLV, iLV, Version(0,1,0,0));

public:
  EmotLV(sUIActionNotify_createWindow* an, int w, int h);
  virtual ~EmotLV();

public:
  struct sEmotPackInfo {
    oImage image;
    bool checked;
    UINT id;
    eMSet* set;

    sEmotPackInfo(bool checked, eMSet* _set, oImage img): checked(checked), image(img), set(_set), id(-1) { }
    sEmotPackInfo(): checked(false) { }
  };

  typedef std::vector<ListWnd::Item*> tItems;

public:
  virtual UINT addItem(sEmotPackInfo* pack);
  virtual bool moveItem(UINT id, int pos);
  virtual void removeItem(UINT id);
  virtual void removeAllItems();
  virtual int itemsCount();
  virtual ListWnd::oItem getItem(UINT id);
  virtual sEmotPackInfo* getEPI(UINT id);

  virtual void saveState() {
    if (!itemsCount()) return;

    for (UINT i = 0; i < itemsCount(); i++) {
      sEmotPackInfo* info = getEPI(i);
      info->set->setEnabled(info->checked);
      info->set->setPos(info->id);
    }
  }

protected:
  void onMouseMove(int vkey, const Stamina::Point &pos);
  void onMouseUp(int vkey, const Stamina::Point &pos);
  void onMouseDown(int vkey, const Stamina::Point &pos);
  void onKeyDown(int vkey, int info);

protected:
  tItems _items;

  oImage _checked;
  oImage _unchecked;

  bool draged;
  UINT draged_id;
  int mmitem;

  HFONT hFontBold;
  HFONT hFont;

public:
  class EmotPackInfoItem: public ListWnd::EntryImpl {
  public:
    STAMINA_OBJECT_CLASS_VERSION(EmotPackInfoItem, ListWnd::EntryImpl, Version(0,1,0,0));

    EmotPackInfoItem(EmotLV* parent, sEmotPackInfo* emotInfo): _emotInfo(emotInfo) {
      _check = new DrawableButtonBasic(Rect(0,0,16,16), emotInfo->checked ? parent->_checked : parent->_unchecked);
    }

    Size getMinSize();
    Size getMaxSize();
    Size getQuickSize();
    Size getEntrySize(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent, Size fitIn);

    void paintEntry(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent);
    void drawInfo(EmotLV* elv, Rect& rc);
    int sizeInfo(EmotLV* elv, Rect& rc);
    void resizeItems(EmotLV* elv, ListWnd::Item* item);

    virtual void switchState(ListWnd::ListView* lv) {
      EmotLV* elv = (EmotLV*)lv;

      _check->setImage(_emotInfo->checked ? elv->_unchecked.get() : elv->_checked.get());
      _emotInfo->checked = !_emotInfo->checked;
      refreshEntry(lv, Stamina::ListWnd::RefreshFlags::refreshPaint);
      touchConfigWnd();
    }

    string getItemText() {
      eMSet* set = _emotInfo->set;
      string text;

      if (set->getUrl().length()) text += "URL: " + set->getUrl() + "\n";
      if (set->getDescription().length()) text += "Opis: " + set->getDescription();
      if (text.length()) text = rtrim(text.c_str(), "\n");

      return text;
    }

    bool onMouseDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    bool onMouseUp(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    bool onMouseMove(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    bool onMouseDblClk(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    bool onKeyDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, int info);

    sEmotPackInfo* getEmotPackInfo();

  protected:
    sEmotPackInfo* _emotInfo;
    oDrawableButton _check;
    CriticalSection _lock;
  };
};

#endif  // __EMOT_UI_H__