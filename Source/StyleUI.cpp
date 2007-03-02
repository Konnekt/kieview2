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

#include "stdafx.h"

#include "ToolTipIE.h"
#include "StyleUI.h"

StyleLV::StyleLV(sUIActionNotify_createWindow* an, int w, int h): iLV(an, w, h) {
  _previewBtn = new DrawableButtonBasic(Rect(0,0,16,16), new Icon((HICON) ICMessage(IMI_ICONGET, kIEview2::ico::preview, IML_16), false));
  _tip = new ToolTipX::ToolTip(getHwnd(), 0);
  _last_checked = -1;
}

StyleLV::~StyleLV() {
  _tip->reset();
  delete _tip;
}

UINT StyleLV::addItem(sStylePackInfo *s) {
  ListWnd::oItem item = insertEntry(new StyleInfoItem(this, s));
  UINT id = getItemIndex(item);
  getSPI(getItemIndex(item))->id = id;

  if (getSPI(id)->used) {
    selectItem(id);
  }
  return id;
}

bool StyleLV::selectItem(UINT id, bool value) {
  if (_last_checked != -1) {
    if (iLV::selectItem(_last_checked, false)) {
      getSPI(_last_checked)->used = false;
    }
  }
  if (iLV::selectItem(id, true)) {
    getSPI(id)->used = true;
    _last_checked = id;
    return true;
  }
  return false;
}

StyleLV::sStylePackInfo* StyleLV::getSPI(UINT id) {
  ListWnd::oItem item = this->getItem(id);

  if (item) {
    StyleInfoItem* si = (StyleInfoItem*) item->getEntry().get();
    return si->getStylePackInfo();
  }
  return NULL;
}

StyleLV::sStylePackInfo* StyleLV::StyleInfoItem::getStylePackInfo() {
  return _styleInfo;
}

Size StyleLV::StyleInfoItem::getMinSize() {
  return Size(30, 32);
}

Size StyleLV::StyleInfoItem::getMaxSize() {
  return Size(0x7FFF, 32);
}

Size StyleLV::StyleInfoItem::getQuickSize() {
  return Size(0, 32);
}

Size StyleLV::StyleInfoItem::getEntrySize(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent, Size fitIn) {
  LockerCS locker(_lock);

  if (li->isActive()) {
    return Size(fitIn.w, sizeInfo((StyleLV*)lv, Rect(0, 0, fitIn.w, 0)) + 22);
  }
  return Size(fitIn.w, 22);
}

bool StyleLV::StyleInfoItem::onMouseDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  LockerCS locker(_lock);

  StyleLV* slv = (StyleLV*)lv;

  if (vkey == VK_LBUTTON) {
    Point p = pos;
    p.x -= slv->getScrollPos().x;
    p.y -= slv->getScrollPos().y;

    Rect rc = lv->itemToClient(li->getRect());

    Point rcp(rc.left + 3, rc.top + 3);
    _selectBtn->setPos(rcp);

    if (_selectBtn->hitTest(p)) {
      slv->selectItem(slv->getItemIndex(li));
      return false;
    }

    if (_styleInfo->set->hasPreview()) {
      Point rct(rc.right - 19, rc.top + 3);
      slv->_previewBtn->setPos(rct);

      if (slv->_previewBtn->hitTest(p)){
        slv->_tip->setTip(slv->getHwnd(), oTarget((iTarget*) new TargetWindow(new TipOnlyImage(loadImageFromFile(_styleInfo->set->getPreview().c_str())), slv->getHwnd())), true);
        slv->_tip->setPos(Point(), true, ToolTipX::enPositioning::positionAuto, ToolTipX::enPlacement::pRightBottom);
        slv->_tip->show();
        return false;
      }
    }
  }
  return true;
}

void StyleLV::StyleInfoItem::paintEntry(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent) {
  LockerCS locker(_lock);

  StyleLV* slv = (StyleLV*)lv;
  HDC dc = lv->getDC();
  Rect rc = lv->itemToClient(li->getRect());

  string name = _styleInfo->set->getName();
  if (_styleInfo->set->getVersion().length()) {
    name += " (" + _styleInfo->set->getVersion() + ")";
  }

  Rect rci = rc;
  rci.left += 25;
  rci.top += 2;
  oImage gradient;

  SetTextColor(dc, RGB(255, 255, 255));
  COLORREF textColor;

  /* predefiniowane kolorki */
  if (slv->isEnabled()) {
    if (li->isActive()) {
      gradient = Stamina::UI::createSimpleGradient(RGB(0,0,0), RGB(60,60,60), Size(rc.width(), rc.height()));
      textColor = RGB(255,255,255);
    } else {
      if (isSelected()) {
        gradient = Stamina::UI::createSimpleGradient(RGB(190,252,122), RGB(219,253,181), Size(rc.width(), rc.height()));
      } else {
       gradient = Stamina::UI::createSimpleGradient(RGB(220,220,220), RGB(240,240,240), Size(rc.width(), rc.height()));
      }
      textColor = RGB(40,40,40);
    }
  } else {
    gradient = Stamina::UI::createSimpleGradient(RGB(193,193,193), RGB(223,223,223), Size(rc.width(), rc.height()));
    textColor = RGB(40,40,40);
  }

  /*
  // kod z Actio
  brushSelected = createSimpleGradient(blendRGB(active, window, 0xD0), active, Size(10, entryCallHeight), 0, 0x80);
  brushActive = createSimpleGradient(blendRGB(active, window, 0x30), blendRGB(active, window, 0x80), Size(10, entryCallHeight), 0, 0xA0);
  brushWaiting = createSimpleGradient(window, blendRGB(active, window, 0x30), Size(10, entryCallHeight), 0, 0xA0);

  brushCollection = createSimpleGradient(button, blendRGB(button, window, 0xA0), Size(10, entryCollectionHeight), 0, 0x40);
  brushCollectionSelected = createSimpleGradient(blendRGB(active, window, 0x80), active, Size(10, entryCollectionHeight), 0, 0x10);

  brushSelectedEntry = createSimpleGradient(blendRGB(active, window, 0x80), active, Size(10, entryHeight*2), 0, /* 0x10 *\/ 0x0;
  brushInfoEntry = createSimpleGradient(blendRGB(button, window, 0x40), blendRGB(button, window, 0x10), Size(10, entryInfoMinimum*2), 0, 0xA0);

  // kolorki z palety systemowej
  COLORREF active = GetSysColor(COLOR_HIGHLIGHT);
  COLORREF window = GetSysColor(COLOR_WINDOW);
  COLORREF inactive = GetSysColor(COLOR_INACTIVECAPTION);
  COLORREF button = GetSysColor(COLOR_BTNFACE);

  COLORREF colorSelectedText = GetSysColor(COLOR_HIGHLIGHTTEXT);
  COLORREF colorText = GetSysColor(COLOR_WINDOWTEXT);

  if (li->isActive()) {
    gradient = createSimpleGradient(blendRGB(active, window, 0x80), active, Size(rc.width(), rc.height()));
    textColor = colorSelectedText;
  } else {
    if (_emotInfo->checked) {
      gradient = createSimpleGradient(blendRGB(button, window, 0xA0), button, Size(rc.width(), rc.height()));
      textColor = colorText;
    } else {
      gradient = createSimpleGradient(blendRGB(button, window, 0x10), blendRGB(button, window, 0x40), Size(rc.width(), rc.height()));
      textColor = colorText;
    }
  }
  */
  gradient->draw(dc, rc.getLT());

  Rect rctxt = rc;
  rctxt.left += 25;
  rctxt.top += 4;
  rctxt.right -= 3 + 16 + 5;
  SetBkMode(dc, TRANSPARENT);

  Stamina::Point p = Point(0,0);

  Point rcp(rc.left + 3, rc.top + 3);
  _selectBtn->setPos(rcp);
  _selectBtn->draw(dc, p);

  if (_styleInfo->set->hasPreview()) {
    Point rct(rc.right - 19, rc.top + 3);
    slv->_previewBtn->setPos(rct);
    slv->_previewBtn->draw(dc, p);
  }

  HFONT oldFont = (HFONT)SelectObject(dc, slv->_fontBold);
  COLORREF oldTextColor = SetTextColor(dc, textColor);
  rctxt.right -= 19;
  DrawTextA(dc, name.c_str(), -1, rctxt.ref(), DT_NOPREFIX | DT_END_ELLIPSIS | DT_SINGLELINE);
  SelectObject(dc, oldFont);
  rctxt.top += 16;
  rctxt.right += 19;

  if (li->isActive()) {
    drawInfo(slv, rctxt);
  }
  SetTextColor(dc, oldTextColor);
  lv->releaseDC(dc);
}
