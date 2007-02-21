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
#include "styleUI.h"
#include "iLV.h"

StyleLV::StyleLV(sUIActionNotify_createWindow* an, int w, int h): iLV(an, w, h) {
  _checked = new Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::checked, IML_16), false);
  _unchecked = new Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::unchecked, IML_16), false);

  hFontBold = CreateFont(-11, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
    CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");
  hFont = CreateFont(-11, 0, 0, 0, FW_MEDIUM, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
    CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");

  _last_checked = -1;
}


StyleLV::~StyleLV() {
  removeAll();

  DeleteObject(hFontBold);
  DeleteObject(hFont);

  removeValidLV(this);
}

UINT StyleLV::addItem(sStylePackInfo *s) {
  ListWnd::oItem item = insertEntry(new StyleInfoItem(this, s));
  UINT id = getItemIndex(item);
  getSPI(getItemIndex(item))->id = id;
  if (getSPI(id)->used) {
    selectRadio(id);
  }
  return id;
}

ListWnd::oItem StyleLV::getItem(UINT id) {
  if (id >= itemsCount()) return ListWnd::oItem(NULL);
  int i = 0;

  for (ListWnd::ItemList::iterator it = getItemList().begin(); it != getItemList().end(); it++, i++) { 
    if (i == id) {
      return *it;
    }
  }

  return ListWnd::oItem(NULL);
}

void StyleLV::removeItem(UINT id) {
  if (id >= itemsCount()) return;

  getSPI(id)->id = -1;

  ListWnd::oItem item = getItem(id);
  removeEntry(item->getEntry());
}

void StyleLV::removeAllItems() {
  for (int i = 0; i < itemsCount(); i++) {
    getSPI(i)->id = -1;
  }
  removeAll();
}

int StyleLV::itemsCount() {
  return this->getItemList().count();
}

void StyleLV::selectRadio(UINT id) {
  ListWnd::oItem item = getItem(id);
  ListWnd::oItem lastItem = getItem(_last_checked);
  if (item) {
    if (_last_checked != -1 && lastItem) {
      getSPI(_last_checked)->used = false;
      StyleInfoItem* s = (StyleInfoItem*)lastItem->getEntry().get();
      s->_radio->setImage(_unchecked);
      lastItem->getEntry()->refreshEntry(this, Stamina::ListWnd::RefreshFlags::refreshPaint);
    }

    getSPI(id)->used = true;
    StyleInfoItem* s = (StyleInfoItem*)item->getEntry().get();
    s->_radio->setImage(_checked);
    item->getEntry()->refreshEntry(this, Stamina::ListWnd::RefreshFlags::refreshPaint);
    _last_checked = id;

    touchConfigWnd();
  }
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

  if (li->isActive()) 
    return Size(fitIn.w, sizeInfo((StyleLV*)lv, Rect(0, 0, fitIn.w, 0)) + 22);
  return Size(fitIn.w, 22);
}

void StyleLV::StyleInfoItem::drawInfo(StyleLV* slv, Rect& rc) {
  TplSet* set = _styleInfo->set;
  HDC dc = slv->getDC();
  string textA = getItemText();
  RECT rcz = *rc.ref();

  HFONT hOldFont = (HFONT)SelectObject(dc, slv->hFont);
  DrawText(dc, textA.c_str(), -1, &rcz, DT_WORDBREAK);
  SelectObject(dc, hOldFont);
  slv->releaseDC(dc);
}

int StyleLV::StyleInfoItem::sizeInfo(StyleLV* slv, Rect& rc) {
  TplSet* set = _styleInfo->set;
  HDC dc = slv->getDC();
  string textA = getItemText();
  RECT rcz = {0, 0, rc.width(), 0};

  HFONT hOldFont = (HFONT)SelectObject(dc, slv->hFont);
  DrawText(dc, textA.c_str(), -1, &rcz, DT_CALCRECT | DT_WORDBREAK);
  SelectObject(dc, hOldFont);
  slv->releaseDC(dc);

  return rcz.bottom;
}

bool StyleLV::StyleInfoItem::onMouseDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  LockerCS locker(_lock);

  StyleLV* slv = (StyleLV*)lv;

  if (vkey == VK_LBUTTON) {
    Point p = pos;
    p.x -= slv->getScrollPos().x;
    p.y -= slv->getScrollPos().y;

    Rect rc = lv->itemToClient(li->getRect());

    Rect rcradio = rc;
    rcradio.left += 3;
    rcradio.top += 3;
    _radio->setPos(rcradio.getLT());

    if (_radio->hitTest(p)) {
      slv->selectRadio(slv->getItemIndex(li));
      return false;
    }
  }
  return true;
}

bool StyleLV::StyleInfoItem::onMouseDblClk(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  LockerCS locker(_lock);

  StyleLV* slv = (StyleLV*) lv;

  if (vkey == VK_LBUTTON) {
    slv->selectRadio(slv->getItemIndex(li));
    return false;
  }
  return true;
}

bool StyleLV::StyleInfoItem::onKeyDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, int info) {
  LockerCS locker(_lock);

  StyleLV* slv = (StyleLV*) lv;

  if (vkey == VK_SPACE) {
    slv->selectRadio(slv->getItemIndex(li));
    return false;
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
  if (slv->_enabled) {
    if (li->isActive()) {
      gradient = Stamina::UI::createSimpleGradient(RGB(0,0,0), RGB(60,60,60), Size(rc.width(), rc.height()));
      textColor = RGB(255,255,255);
    } else {
      if (_styleInfo->used) {
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

  Rect rcradio = rc;
  rcradio.left += 3;
  rcradio.top += 3;
  _radio->setPos(rcradio.getLT());
  _radio->draw(dc, p);

  HFONT hOldFont = (HFONT)SelectObject(dc, slv->hFontBold);
  COLORREF oldTextColor = SetTextColor(dc, textColor);
  DrawTextA(dc, name.c_str(), -1, rctxt.ref(), DT_NOPREFIX | DT_END_ELLIPSIS | DT_SINGLELINE);
  SelectObject(dc, hOldFont);
  rctxt.top += 16;

  if (li->isActive()) {
    drawInfo(slv, rctxt);
  }
  SetTextColor(dc, oldTextColor);
  lv->releaseDC(dc);
}
