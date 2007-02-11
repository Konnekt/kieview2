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

class EmotLV: public ListWnd::ListView {
public:
  EmotLV(int x, int y, int w, int h, HWND parent, HMENU id);
  virtual ~EmotLV();
  typedef std::vector<ListWnd::Item*> tItems;

  class EmotPackInfoItem;

  struct sEmotPackInfo {
    UINT id;
    string name;
    bool checked;
    Stamina::UI::oImage image;
    string authors;
    string releasedTime;
    string other;

    sEmotPackInfo(): id(Stamina::random()), name("unknown"), checked(false) { }
    sEmotPackInfo(string name, bool checked): id(Stamina::random()), name(name), checked(checked) { }
  };

  void onMouseUp(int vkey, const Stamina::Point &pos);
  void onMouseMove(int vkey, const Stamina::Point &pos);

  virtual void setPos(int x, int y);
  virtual void setSize(int w, int h);

  virtual UINT addItem(sEmotPackInfo *s);
  virtual bool moveItem(UINT id, int pos);
  virtual int removeItem(UINT id);
  virtual int itemsCount();
  virtual ListWnd::oItem getItem(UINT id);
  virtual sEmotPackInfo* getEPI(UINT id);

protected:
  tItems _items;

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

    bool onMouseDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    bool onMouseUp(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    bool onMouseMove(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    bool onKeyDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, int info);

    sEmotPackInfo* getEmotPackInfo();

  protected:
    sEmotPackInfo* _emotInfo;

    Stamina::UI::oDrawableButton _check;
    Stamina::UI::oDrawableButton _inform;
  };
};

#endif  // __EMOT_UI_H__