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

StyleLV::tStyleLVs StyleLV::_lvs;

StyleLV::StyleLV(int x, int y, int w, int h, HWND parent, HMENU id): ListWnd::ListView(x, y, w, h, parent, id), _enabled(true) {
  alwaysShowScrollbars(false, false);

  _lastProc = (WNDPROC)SetWindowLong(getHwnd(), GWL_WNDPROC, (LONG) sMsgProc);

  _checked = new Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::checked, IML_16), false);
  _unchecked = new Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::unchecked, IML_16), false);

  hFontBold = CreateFont(-11, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
    CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");
  hFont = CreateFont(-11, 0, 0, 0, FW_MEDIUM, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
    CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");

  _last_checked = -1;
  _lvs.push_back(this);
}

LRESULT CALLBACK StyleLV::sMsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  StyleLV* slv = (StyleLV*)ListWnd::ListView::fromHWND(hwnd);
  switch(msg) {
    case WM_ENABLE:
      slv->_enabled = (bool)wParam;
      if (IsWindowVisible(hwnd)) {
        slv->repaintClient();
      }
    break;
    case WM_DESTROY:
      WNDPROC proc = slv->_lastProc;
      SetWindowLong(slv->getHwnd(), GWL_WNDPROC, (LONG) proc);
      return CallWindowProc(proc, hwnd, msg, wParam, lParam);
    break;
  }
  return CallWindowProc(slv->_lastProc, hwnd, msg, wParam, lParam);
}

bool StyleLV::isVaildLV(StyleLV* lv) {
  for (tStyleLVs::iterator it = _lvs.begin(); it != _lvs.end(); it++) {
    if ((*it) == lv) return true;
  }
  return false;
}

void StyleLV::onMouseUp(int vkey, const Stamina::Point &pos) {
  ReleaseCapture();
  ListWnd::ListView::onMouseUp(vkey, pos);
  scrollToActive(false);
  this->refreshItems();
}

void StyleLV::onMouseDown(int vkey, const Stamina::Point &pos) {
  ListWnd::ListView::onMouseDown(vkey, pos);
  if (_last_checked != -1) {
    _items[_last_checked]->setRefreshFlag(ListWnd::refreshSize);
  }
  getRootItem()->setFlag(ListWnd::flagSubitemsChanged, true, this);
  refreshItems();
}

void StyleLV::onKeyDown(int vkey, int info) {
  ListWnd::ListView::onKeyDown(vkey, info);
  getRootItem()->setFlag(ListWnd::flagSubitemsChanged, true, this);
  refreshItems();
  scrollToActive(false);
}

StyleLV::~StyleLV() {
  removeAll();

  DeleteObject(hFontBold);
  DeleteObject(hFont);

  for (tStyleLVs::iterator it = _lvs.begin(); it != _lvs.end(); it++) {
    if ((*it) == this) {
      _lvs.erase(it); break;
    }
  }
}

void StyleLV::setPos(int x, int y) {
  SetWindowPos(getHwnd(), 0, x, y, 0, 0, SWP_NOSIZE);
}

void StyleLV::setSize(int w, int h) {
  SetWindowPos(getHwnd(), 0, 0, 0, w, h, SWP_NOMOVE);
}

bool StyleLV::isEnabled() {
  return IsWindowEnabled(getHwnd());
}

void StyleLV::setEnabled(bool enabled) {
  EnableWindow(getHwnd(), enabled);
}

UINT StyleLV::addItem(sStylePackInfo *s) {
  _items.push_back(insertEntry(new StyleInfoItem(this, s)).get());
  getSPI(_items.size() - 1)->id = _items.size() - 1;
  if (getSPI(_items.size() - 1)->used) {
    selectRadio(_items.size() - 1);
  }
  return _items.size() - 1;
}

void StyleLV::removeItem(UINT id) {
  removeEntry(_items[id]->getEntry());
  getSPI(id)->id = -1;
  tItems::iterator it = _items.begin() + id;
  _items.erase(it);
}

void StyleLV::removeAllItems() {
  for (int i = 0; i < itemsCount(); i++) {
    getSPI(i)->id = -1;
  }
  removeAll();
  _items.clear();
}

int StyleLV::itemsCount() {
  return _items.size();
}

ListWnd::oItem StyleLV::getItem(UINT id) {
  if (id > _items.size()) return NULL;
  return _items[id];
}

void StyleLV::selectRadio(UINT id) {
  if (id >= itemsCount()) return;

  if (_last_checked != -1) {
    getSPI(_last_checked)->used = false;
    StyleInfoItem* s = (StyleInfoItem*)_items[_last_checked]->getEntry().get();
    s->_radio->setImage(_unchecked);
    _items[_last_checked]->getEntry()->refreshEntry(this, Stamina::ListWnd::RefreshFlags::refreshPaint);
  }

  StyleInfoItem* s = (StyleInfoItem*)_items[id]->getEntry().get();
  s->_radio->setImage(_checked);
  getSPI(id)->used = true;
  _items[id]->getEntry()->refreshEntry(this, Stamina::ListWnd::RefreshFlags::refreshPaint);
  _last_checked = id;

  touchConfigWnd();
}

StyleLV::sStylePackInfo* StyleLV::getSPI(UINT id) {
  if (id > _items.size()) return NULL;
  return ((StyleInfoItem*)_items[id]->getEntry().get())->getStylePackInfo();
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
  sSet* set = _styleInfo->set;
  HDC dc = slv->getDC();
  string textA = getItemText();
  RECT rcz = *rc.ref();

  HFONT hOldFont = (HFONT)SelectObject(dc, slv->hFont);
  DrawText(dc, textA.c_str(), -1, &rcz, DT_WORDBREAK);
  SelectObject(dc, hOldFont);
  slv->releaseDC(dc);
}

int StyleLV::StyleInfoItem::sizeInfo(StyleLV* slv, Rect& rc) {
  sSet* set = _styleInfo->set;
  HDC dc = slv->getDC();
  string textA = getItemText();
  RECT rcz = {0, 0, rc.width(), 0};

  HFONT hOldFont = (HFONT)SelectObject(dc, slv->hFont);
  DrawText(dc, textA.c_str(), -1, &rcz, DT_CALCRECT | DT_WORDBREAK);
  SelectObject(dc, hOldFont);
  slv->releaseDC(dc);

  return rcz.bottom;
}

void StyleLV::StyleInfoItem::resizeItems(StyleLV* slv, ListWnd::Item* item) {
  ListWnd::oItem lastItem = slv->getActiveItem();
  slv->setActiveItem((ListWnd::Item*)item);
  if (lastItem.isValid()) {
    lastItem->setRefreshFlag(ListWnd::RefreshFlags::refreshAll);
  }
  item->setFlag(ListWnd::ItemFlags::flagResize, true);
  item->setFlag(ListWnd::ItemFlags::flagRepos, true);
  item->setRefreshFlag(ListWnd::RefreshFlags::refreshAll);
  slv->getRootItem()->setFlag(ListWnd::ItemFlags::flagSubitemsChanged, true);
  slv->getRootItem()->needResize();

  slv->refreshItems();
  slv->updateScrollbars();
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
    } else {
      resizeItems(slv, li.get());
      return false;
    }
  }
  return true;
}

bool StyleLV::StyleInfoItem::onMouseDblClk(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  LockerCS locker(_lock);

  StyleLV* slv = (StyleLV*)lv;

  if (vkey == VK_LBUTTON) {
    slv->selectRadio(slv->getItemIndex(li));
    return false;
  }
  return true;
}

bool StyleLV::StyleInfoItem::onKeyDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, int info) {
  LockerCS locker(_lock);

  StyleLV* slv = (StyleLV*)lv;
  slv->getRootItem()->needResize();
  slv->refreshItems();

  if (vkey == VK_SPACE) {
    slv->selectRadio(slv->getItemIndex(li));
    return false;
  } else if (vkey == VK_UP){
    int id = lv->getItemIndex(li);
    if(--id >= 0) {
      resizeItems(slv, slv->_items[id]);
    }
  } else if (vkey == VK_DOWN){
    int id = lv->getItemIndex(li);
    if(++id < slv->itemsCount()) {
      resizeItems(slv, slv->_items[id]);
    }
  } else if (vkey == VK_HOME) {
    if (slv->itemsCount()) {
      resizeItems(slv, slv->_items[0]);
    }
  } else if (vkey == VK_END) {
    if (slv->itemsCount()) {
      resizeItems(slv, slv->_items[slv->itemsCount() - 1]);
    }
  }
  slv->getRootItem()->setFlag(ListWnd::flagSubitemsChanged, true, lv);
  slv->refreshItems();
  return false;
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
