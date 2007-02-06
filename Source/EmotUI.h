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

class EmotPackInfoLV {
public:
  typedef std::vector<ListWnd::Item*> tItems;

  class EmotPackInfoItem;

  struct sEmotPackInfo {
    string name;
    bool checked;
    Stamina::UI::oImage image;
    string authors;
    string releasedTime;
    string other;

    sEmotPackInfo(): name("unknown"), checked(false) { }
    sEmotPackInfo(string name, bool checked, Stamina::UI::oImage& image): name(name), checked(checked), image(image) { }
  };

public:
  EmotPackInfoLV(HWND parent, int x, int y, int w, int h);
  virtual ~EmotPackInfoLV();

  virtual ListWnd::ListView* getLV();

  virtual void setPos(int x, int y);
  virtual void setSize(int w, int h);

  virtual int addItem(sEmotPackInfo *s);
  virtual bool moveItem(UINT id, int pos);
  virtual int removeItem(UINT id);
  virtual int itemsCount();
  virtual EmotPackInfoItem* getItem(UINT id);

  tItems _items;

  Stamina::UI::oImage _checked;
  Stamina::UI::oImage _unchecked;
  Stamina::UI::oImage _inform;

  Stamina::UI::ToolTipX::ToolTip* _tooltip;

protected:
  ListWnd::ListView* _lv;
  HFONT hFont;

public:
  class EmotPackInfoItem: public ListWnd::EntryImpl {
  public:
    STAMINA_OBJECT_CLASS_VERSION(EmotPackInfoItem, ListWnd::EntryImpl, Version(0,1,0,0));

    EmotPackInfoLV::EmotPackInfoItem(EmotPackInfoLV* parent, EmotPackInfoLV::sEmotPackInfo* emotInfo): _parent(parent), _emotInfo(emotInfo) {
      _check = new Stamina::UI::DrawableButtonBasic(Rect(0,0,16,16), emotInfo->checked ? _parent->_checked : _parent->_unchecked);
      _inform = new Stamina::UI::DrawableButtonBasic(Rect(0,0,16,16), _parent->_inform);
    }

    Size getMinSize();
    Size getMaxSize();
    Size getQuickSize();
    Size getEntrySize(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent, Size fitIn);

    void paintEntry(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent);
    void showToolTip(Point& pos);

  protected:
    EmotPackInfoLV* _parent;
    sEmotPackInfo* _emotInfo;

    Stamina::UI::oDrawableButton _check;
    Stamina::UI::oDrawableButton _inform;
    bool onMouseDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    bool onKeyUp(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, int info);
  };
};

#endif  // __EMOT_UI_H__