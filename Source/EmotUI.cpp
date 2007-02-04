#include "stdafx.h"
#include "emotUI.h"

EmotPackInfoLV::EmotPackInfoLV(HWND parent, int x, int y, int w, int h) {
  _lv = new ListWnd::ListView(x, y, w, h, parent, 0);

  _checked = new Stamina::UI::Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::checked, IML_16), false);
  _unchecked = new Stamina::UI::Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::unchecked, IML_16), false);
}

void EmotPackInfoLV::setPos(int x, int y) {
  MoveWindow(_lv->getHwnd(), x, y, 0, 0, SWP_NOSIZE);
}

void EmotPackInfoLV::setSize(int w, int h) {
  MoveWindow(_lv->getHwnd(), 0, 0, w, h, SWP_NOMOVE);
}

int EmotPackInfoLV::addItem(sEmotPackInfo *s) {
  _items.push_back(_lv->insertEntry(new EmotPackInfoItem(this, s)).get());
  return _items.size() - 1;
}

bool EmotPackInfoLV::moveItem(UINT id, int pos) {
  if (id > _items.size()) return false;

  ListWnd::oEntry entry = _items[id]->getEntry();
  _lv->removeEntry(entry, false);
  _items[id] = _lv->insertEntry(entry, pos).get();
  return true;
}

int EmotPackInfoLV::removeItem(UINT id) {
  _lv->removeEntry(_items[id]->getEntry());
  tItems::iterator it = _items.begin() + id;
  _items.erase(it);
  return id;
}

int EmotPackInfoLV::itemsCount() {
  return _items.size();
}

EmotPackInfoLV::EmotPackInfoItem* EmotPackInfoLV::getItem(UINT id) {
  if (id > _items.size()) return NULL;
  return (EmotPackInfoItem*)_items[id]->getEntry().get();
}



Size EmotPackInfoLV::EmotPackInfoItem::getMinSize() {
  return Size(30, 38);
}

Size EmotPackInfoLV::EmotPackInfoItem::getMaxSize() {
  return Size(0x7FFF, 38);
}

Size EmotPackInfoLV::EmotPackInfoItem::getQuickSize() {
  return Size(0, 38);
}

Size EmotPackInfoLV::EmotPackInfoItem::getEntrySize(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent, Size fitIn) {
  return Size(fitIn.w, 16);
}

bool EmotPackInfoLV::EmotPackInfoItem::onMouseDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  if (_check->hitTest(pos)) {
    if (_emotInfo->checked) {
      _check->setImage(_parent->_unchecked.get());
      this->_emotInfo->checked = false;
      this->refreshEntry(lv, Stamina::ListWnd::RefreshFlags::refreshPaint);
    } else {
      _check->setImage(_parent->_checked.get());
      this->_emotInfo->checked = true;
      this->refreshEntry(lv, Stamina::ListWnd::RefreshFlags::refreshPaint);
    }
    return 0;
  } else {
    return 1;
  }
}

void EmotPackInfoLV::EmotPackInfoItem::paintEntry(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent) {
  HDC dc = lv->getDC();
  Rect rc = lv->itemToClient(li->getRect());
  string name = this->_emotInfo->name;

  HBRUSH hBrush;

  Rect rci = rc;
  rci.left+= 20;

  if (li->isSelected()) {
    hBrush = CreateSolidBrush(RGB(162, 186, 251));
    SetTextColor(dc, 0xFF);
  } else {
    hBrush = CreateSolidBrush(RGB(254, 241, 216));
    SetTextColor(dc, 0);
  }
  HBRUSH hOldBrush = (HBRUSH)SelectObject(dc, hBrush);
  FillRect(dc, rc.ref(), hBrush);
  DeleteObject(SelectObject(dc, hOldBrush));
  _emotInfo->image->draw(dc, rci.getLT());
  Rect rctxt = rc;
  rctxt.left+= 40;

  SetBkMode(dc, TRANSPARENT);
  Rect rccheck = rc;
  rccheck.left;
  rccheck.top+=0;
  _check->setPos(rccheck.getLT());
  Stamina::Point p = Point(0,0);
  _check->draw(dc, p);
  DrawTextA(dc, name.c_str(), -1, rctxt.ref(), DT_NOPREFIX | DT_WORDBREAK);

  lv->releaseDC(dc);
}