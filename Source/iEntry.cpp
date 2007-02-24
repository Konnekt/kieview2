/**
  *  Base ListView Entry class (kIEv2 flavor)
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 ursus6
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 233 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2007-02-22 19:25:37 +0100 (Cz, 22 lut 2007) $
  */

#include "stdafx.h"
#include "iEntry.h"

namespace kIEview2 {
  void iEntry::drawInfo(iLV* lv, Rect& rc) {
    string text = getText();
    HDC dc = lv->getDC();
    RECT rcz = *rc.ref();

    HFONT oldFont = (HFONT) SelectObject(dc, lv->_fontNormal);
    DrawText(dc, text.c_str(), -1, &rcz, DT_WORDBREAK);
    SelectObject(dc, oldFont);
    lv->releaseDC(dc);
  }

  int iEntry::sizeInfo(iLV* lv, Rect& rc) {
    string text = getText();
    HDC dc = lv->getDC();
    RECT rcz = {0, 0, rc.width() - 25, 0};

    HFONT oldFont = (HFONT) SelectObject(dc, lv->_fontNormal);
    DrawText(dc, text.c_str(), -1, &rcz, DT_CALCRECT | DT_WORDBREAK);
    SelectObject(dc, oldFont);
    lv->releaseDC(dc);

    return rcz.bottom;
  }

  void iEntry::setSelected(iLV* lv, bool value) {
    _selectBtn->setImage(value ? lv->_checked : lv->_unchecked);
    _selected = value;
    refreshEntry(lv, ListWnd::RefreshFlags::refreshPaint);
    touchConfigWnd();
  }

  bool iEntry::onMouseDblClk(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
    LockerCS locker(_lock);

    if (vkey == VK_LBUTTON) {
      switchState((iLV*) lv);
      return false;
    }
    return true;
  }
  bool iEntry::onKeyDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, int info) {
    LockerCS locker(_lock);

    if (vkey == VK_SPACE) {
      switchState((iLV*) lv);
      return false;
    }
    return true;
  }
}