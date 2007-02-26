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

#include "kIEview2.h"
#include "iLV.h"
#include "iEntry.h"
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

public:
  struct sEmotPackInfo {
    oImage image;
    bool checked;
    UINT id;
    eMSet* set;

    sEmotPackInfo(bool checked, eMSet* _set, oImage img): checked(checked), image(img), set(_set), id(-1) { }
    sEmotPackInfo(): checked(false) { }
  };

public:
  virtual UINT addItem(sEmotPackInfo* pack);
  virtual sEmotPackInfo* getEPI(UINT id);

  virtual void saveState() {
    if (!itemsCount()) return;

    for (UINT i = 0; i < itemsCount(); i++) {
      sEmotPackInfo* info = getEPI(i);
      info->set->setEnabled(info->checked);
      info->set->setPos(info->id);
    }
  }
  virtual void refreshEPIID() {
    for (int i = 0; i < itemsCount(); i++) {
      getEPI(i)->id = i;
    }
  }

protected:
  void onMouseMove(int vkey, const Stamina::Point &pos);
  void onMouseUp(int vkey, const Stamina::Point &pos);

protected:
  bool draged;
  UINT draged_id;
  int mmitem;

public:
  class EmotPackInfoItem: public iEntry {
  public:
    STAMINA_OBJECT_CLASS_VERSION(EmotPackInfoItem, iEntry, Version(0,1,0,0));

    EmotPackInfoItem(EmotLV* parent, sEmotPackInfo* emotInfo): _emotInfo(emotInfo), iEntry(parent, emotInfo->checked) { }

    Size getMinSize();
    Size getMaxSize();
    Size getQuickSize();
    Size getEntrySize(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent, Size fitIn);

    void paintEntry(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent);

    string getText() {
      return _emotInfo->set->getDescription();
    }

    void setSelected(iLV* lv, bool value = true) {
      iEntry::setSelected(lv, value);
      _emotInfo->checked = value;
    }

    bool onMouseDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    bool onMouseUp(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);
    bool onMouseMove(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos);

    sEmotPackInfo* getEmotPackInfo();

  protected:
    sEmotPackInfo* _emotInfo;
  };
};

#endif  // __EMOT_UI_H__
