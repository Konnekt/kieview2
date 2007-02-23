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

#include "kIEview2.h"
#include "Helpers.h"

using namespace Stamina::UI;
using namespace Helpers;
using namespace kIEview2;

namespace kIEview2 {
  class iLV: public ListWnd::ListView {
  public:
    STAMINA_OBJECT_CLASS_VERSION(iLV, ListWnd::ListView, Version(0,1,0,0));

    friend class iEntry;

  public:
    typedef vector<iLV*> tInstances;

  public:
    iLV(sUIActionNotify_createWindow* an, int w, int h);
    virtual ~iLV();

    static bool isValidLV(iLV* lv);
    static void addValidLV(iLV* lv);
    static void removeValidLV(iLV* lv);

    virtual UINT itemsCount() {
      return getItemList().count();
    }
    virtual ListWnd::oItem getItem(UINT id);
    virtual void removeItem(UINT id);
    virtual bool selectItem(UINT id, bool value = true);

    virtual void saveState() = 0;

    virtual void setSize(int w, int h);
    virtual void setPos(int x, int y);

  protected:
    static tInstances _instances;

    oImage _unchecked;
    oImage _checked;

    HFONT _fontNormal;
    HFONT _fontBold;
  };
}

#endif // __ILV_H__