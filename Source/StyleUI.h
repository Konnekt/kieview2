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

#include "kIEview2.h"
#include "iLV.h"
#include "iEntry.h"
#include "TplHandler.h"
#include "Helpers.h"

using namespace Stamina::UI;
using namespace Helpers;
using namespace kIEview2;

class StyleLV: public iLV {
public:
  STAMINA_OBJECT_CLASS_VERSION(StyleLV, iLV, Version(0,1,0,0));

public:
  StyleLV(sUIActionNotify_createWindow* an, int w, int h);
  ~StyleLV();

public:
  struct sStylePackInfo {
    TplSet* set;
    bool used;
    UINT id;

    sStylePackInfo(bool used, TplSet* _set): used(used), set(_set), id(-1) { }
    sStylePackInfo(): used(false) { }
  };

public:
  UINT addItem(sStylePackInfo* pack);
  sStylePackInfo* getSPI(UINT id);
  bool selectItem(UINT id, bool value = true);

  void saveState() {
    if (!itemsCount()) return;

    for (UINT i = 0; i < itemsCount(); i++) {
      sStylePackInfo* info = getSPI(i);
      info->set->setEnabled(info->used);
    }
  }

  void onMouseDown(int vkey, const Point& pos) {
    if (_tip->visible()) _tip->hide();
    ListWnd::ListView::onMouseDown(vkey, pos);
  }
  void onKeyDown(int vkey, int info) {
    if (_tip->visible()) _tip->hide();
    ListWnd::ListView::onKeyDown(vkey, info);
  }

protected:
  oDrawableButton _previewBtn;
  ToolTipX::ToolTip* _tip;
  UINT _last_checked;

public:
  class StyleInfoItem: public iEntry {
  public:
    friend class StyleLV;

  public:
    STAMINA_OBJECT_CLASS_VERSION(StyleInfoItem, iEntry, Version(0,1,0,0));

    StyleInfoItem(StyleLV* parent, sStylePackInfo* styleInfo): _styleInfo(styleInfo), iEntry(parent, styleInfo->used) { }

    Size getMinSize();
    Size getMaxSize();
    Size getQuickSize();
    Size getEntrySize(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent, Size fitIn);

    void paintEntry(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent);

    string getText() {
      return _styleInfo->set->getDescription();
    }

    bool onMouseDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);

    sStylePackInfo* getStylePackInfo();

  protected:
    sStylePackInfo* _styleInfo;
  };
};

#endif  // __STYLE_UI_H__