#include "stdafx.h"
#include "emotUI.h"

EmotPackInfoLV::EmotPackInfoLV(HWND parent, int x, int y, int w, int h) {
  _lv = new ListWnd::ListView(x, y, w, h, parent, 0);
  _lv->alwaysShowScrollbars(false, true);

  _checked = new Stamina::UI::Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::checked, IML_16), false);
  _unchecked = new Stamina::UI::Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::unchecked, IML_16), false);

  hFont = CreateFont(16, 0, 0, 0, FW_MEDIUM, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
      CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Verdana");
}

EmotPackInfoLV::~EmotPackInfoLV() {
  _lv->removeAll();
  DeleteObject(hFont);
  delete _lv;
}

ListWnd::ListView* EmotPackInfoLV::getLV() {
  return _lv;
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
  return Size(30, 32);
}

Size EmotPackInfoLV::EmotPackInfoItem::getMaxSize() {
  return Size(0x7FFF, 32);
}

Size EmotPackInfoLV::EmotPackInfoItem::getQuickSize() {
  return Size(0, 32);
}

Size EmotPackInfoLV::EmotPackInfoItem::getEntrySize(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent, Size fitIn) {
  return Size(fitIn.w, 22);
}

bool EmotPackInfoLV::EmotPackInfoItem::onMouseDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  Point p = pos;
  p.x-= _parent->getLV()->getScrollPos().x;
  p.y-= _parent->getLV()->getScrollPos().y;
  if (_check->hitTest(p)) {
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

bool EmotPackInfoLV::EmotPackInfoItem::onKeyUp(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, int info) {
  if (vkey == VK_SPACE) {
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

  Rect rci = rc;
  rci.left+= 25;
  rci.top+=2;

  Stamina::UI::oImage gradient;

  SetTextColor(dc, RGB(255, 255, 255));

  if (li->isSelected()) {
    gradient = Stamina::UI::createSimpleGradient(RGB(62, 154, 222), RGB(0, 102, 204), Size(rc.right - rc.left, rc.bottom - rc.top));
  } else {
    if (_emotInfo->checked) {
      gradient = Stamina::UI::createSimpleGradient(RGB(102, 204, 51), RGB(0, 153, 0), Size(rc.right - rc.left, rc.bottom - rc.top));
    } else {
      gradient = Stamina::UI::createSimpleGradient(RGB(255, 121, 75), RGB(255, 51, 0), Size(rc.right - rc.left, rc.bottom - rc.top));    
    }
  }
  gradient->draw(dc, rc.getLT());

  _emotInfo->image->draw(dc, rci.getLT());

  Rect rctxt = rc;
  rctxt.left+= 45;
  rctxt.top+= 3;
  SetBkMode(dc, TRANSPARENT);

  Rect rccheck = rc;
  rccheck.left+=3;
  rccheck.top+=3;
  _check->setPos(rccheck.getLT());
  Stamina::Point p = Point(0,0);
  _check->draw(dc, p);

  HFONT hOldFont = (HFONT)SelectObject(dc, _parent->hFont);
  DrawTextA(dc, name.c_str(), -1, rctxt.ref(), DT_NOPREFIX | DT_END_ELLIPSIS | DT_SINGLELINE);
  SelectObject(dc, hOldFont);

  lv->releaseDC(dc);
}