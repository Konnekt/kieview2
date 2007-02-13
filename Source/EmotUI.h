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
#include "Emots.h"

class EmotLV: public ListWnd::ListView {
public:
  STAMINA_OBJECT_CLASS_VERSION(EmotLV, ListWnd::ListView, Version(0,1,0,0));

public:
  typedef vector<EmotLV*> tEmotLVs;

public:
  EmotLV(int x, int y, int w, int h, HWND parent, HMENU id);
  virtual ~EmotLV();
  static bool isVaildLV(EmotLV* lv);

public:
  struct sEmotPackInfo {
    Stamina::UI::oImage image;
    bool checked;
    UINT id;
    eMSet* set;

    sEmotPackInfo(bool checked, eMSet* _set, Stamina::UI::oImage img): checked(checked), image(img), set(_set), id(-1) { }
    sEmotPackInfo(): checked(false) { }
  };

  typedef std::vector<ListWnd::Item*> tItems;

public:
  virtual void setSize(int w, int h);
  virtual void setPos(int x, int y);

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

protected:
  tItems _items;
  static tEmotLVs _lvs;

  Stamina::UI::oImage _checked;
  Stamina::UI::oImage _unchecked;
  Stamina::UI::oImage _inform;

  Stamina::UI::ToolTipX::ToolTip* _tooltip;

  bool draged;
  UINT draged_id;
  int mmitem;

  HFONT hFont;

public:
  class EmotPackInfoItem: public ListWnd::EntryImpl {
  public:
    STAMINA_OBJECT_CLASS_VERSION(EmotPackInfoItem, ListWnd::EntryImpl, Version(0,1,0,0));

    EmotPackInfoItem(EmotLV* parent, sEmotPackInfo* emotInfo): _emotInfo(emotInfo) {
      _check = new Stamina::UI::DrawableButtonBasic(Rect(0,0,16,16), emotInfo->checked ? parent->_checked : parent->_unchecked);
      _inform = new Stamina::UI::DrawableButtonBasic(Rect(0,0,16,16), parent->_inform);
    }

    Size getMinSize();
    Size getMaxSize();
    Size getQuickSize();
    Size getEntrySize(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent, Size fitIn);

    void paintEntry(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent);
    void showToolTip(EmotLV* elv, Point& pos);

    virtual void switchState(ListWnd::ListView* lv) {
      EmotLV* elv = (EmotLV*)lv;

      _check->setImage(_emotInfo->checked ? elv->_unchecked.get() : elv->_checked.get());
      _emotInfo->checked = !_emotInfo->checked;
      refreshEntry(lv, Stamina::ListWnd::RefreshFlags::refreshPaint);
      SendMessage((HWND)UIGroupHandle(sUIAction(0, IMIG_CFGWND)), WM_USER + 18091, 0, 0);
    }

    bool onMouseDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    bool onMouseUp(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    bool onMouseMove(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    bool onMouseDblClk(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    bool onKeyDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, int info);

    sEmotPackInfo* getEmotPackInfo();

  protected:
    sEmotPackInfo* _emotInfo;

    Stamina::UI::oDrawableButton _check;
    Stamina::UI::oDrawableButton _inform;
  };
};

#endif  // __EMOT_UI_H__