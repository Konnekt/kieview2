/**
  *  kIEview2
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 ursus6
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 43 $
  *  @modifiedby   $LastChangedBy: ursus6 $
  *  @lastmodified $Date: 2006-12-31 16:47:02 +0100 (N, 31 gru 2006) $
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

    sEmotPackInfo(): name("unknown"), checked(false) { }
    sEmotPackInfo(string name, bool checked, Stamina::UI::oImage& image): name(name), checked(checked), image(image) { }
  };

public:
  EmotPackInfoLV(HWND parent, int x, int y, int w, int h);
  virtual ~EmotPackInfoLV() {}

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

protected:
  ListWnd::ListView* _lv;

public:
  class EmotPackInfoItem: public ListWnd::EntryImpl {
  public:
    EmotPackInfoLV::EmotPackInfoItem(EmotPackInfoLV* parent, EmotPackInfoLV::sEmotPackInfo* emotInfo): _parent(parent), _emotInfo(emotInfo) {
      _check = new Stamina::UI::DrawableButtonBasic(Rect(0,0,16,16), emotInfo->checked ? _parent->_checked : _parent->_unchecked);
    }

    Size getMinSize();
    Size getMaxSize();
    Size getQuickSize();
    Size getEntrySize(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent, Size fitIn);

    void paintEntry(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent);

  protected:
    EmotPackInfoLV* _parent;
    sEmotPackInfo* _emotInfo;

    Stamina::UI::oDrawableButton _check;
    bool onMouseDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
  };
};

#endif  // __EMOT_UI_H__