#include "stdafx.h"
#include "emotUI.h"

EmotPackInfoLV::EmotPackInfoLV(HWND parent, int x, int y, int w, int h) {
  _lv = new EmotLV(x, y, w, h, parent, 0);
  _lv->alwaysShowScrollbars(false, true);

  _checked = new Stamina::UI::Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::checked, IML_16), false);
  _unchecked = new Stamina::UI::Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::unchecked, IML_16), false);
  _inform = new Stamina::UI::Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::emotsinfo, IML_16), false);

  hFont = CreateFont(16, 0, 0, 0, FW_MEDIUM, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
      CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Verdana");

  _tooltip = new Stamina::UI::ToolTipX::ToolTip(getLV()->getHwnd(), 0);
  Stamina::UI::ToolTipX::Tip* tip = new Stamina::UI::ToolTipX::Tip();
  _tooltip->setTip(tip, false);
  
  draged = false;
  draged_id = 0;
}

EmotPackInfoLV::~EmotPackInfoLV() {
  _lv->removeAll();
  DeleteObject(hFont);
  delete _tooltip;
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

UINT EmotPackInfoLV::addItem(sEmotPackInfo *s) {
  _items.push_back(_lv->insertEntry(new EmotPackInfoItem(this, s)).get());
  return _items.size() - 1;
}

bool EmotPackInfoLV::moveItem(UINT id, int pos) {
  if ((id > _items.size()) || (id == pos)) return false;

  int inc = 0;
  tItems::iterator it = _items.begin();

  if (pos == 0) {
    _items.insert(it, _items[id]);
    it = _items.begin();
    inc = 0;
    while(inc++ < id + 1) it++;
    _items.erase(it);
  } else if (pos == _items.size()) {
    _items.push_back(_items[id]);
    while(inc++ < id) it++;
    _items.erase(it);
    pos-= 1;
  } else if ((id <= _items.size() - 1) && (pos <= _items.size() - 1)) {
    if (id >= pos) {
      while(inc++ < pos) it++;
      _items.insert(it, _items[id]);

      it = _items.begin();
      inc = 0;
      while(inc++ < id + 1) it++;
      _items.erase(it);
    } else {
      while(inc++ < pos) it++;
      _items.insert(it, _items[id]);

      it = _items.begin();
      inc = 0;
      while(inc++ < id) it++;
      _items.erase(it);
      pos-=1;
    }
  } else {
    return false;
  }

  ListWnd::Item* item = _items[pos];
  ListWnd::oEntry entry = item->getEntry();
  _lv->removeEntry(entry, false);
  _items[pos] = _lv->insertEntry(entry, pos).get();

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

ListWnd::oItem EmotPackInfoLV::getItem(UINT id) {
  if (id > _items.size()) return NULL;
  return _items[id];
}

EmotPackInfoLV::sEmotPackInfo* EmotPackInfoLV::getEPI(UINT id) {
  if (id > _items.size()) return NULL;
  return ((EmotPackInfoItem*)_items[id]->getEntry().get())->getEmotPackInfo();
}

EmotPackInfoLV::sEmotPackInfo* EmotPackInfoLV::EmotPackInfoItem::getEmotPackInfo() {
  return _emotInfo;
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
void EmotPackInfoLV::EmotPackInfoItem::showToolTip(Point& pos) {
  string tiptext = stringf("Zestaw emot: <b>%s</b>", _emotInfo->name.c_str());
  if (_emotInfo->authors.length())
    tiptext+= stringf("<br/>Autorzy: <b>%s</b>", _emotInfo->authors.c_str());
  if (_emotInfo->releasedTime.length())
    tiptext+= stringf("<br/>Stworzono: <b>%s</b>", _emotInfo->releasedTime.c_str());
  if (_emotInfo->other.length()) 
    tiptext+= stringf("<br/>Dodatkowe: %s", _emotInfo->other.c_str());

  Stamina::UI::ToolTipX::oTip tip;
  if (_emotInfo->image.isValid()) {
    tip= Stamina::UI::ToolTipX::Tip::textAndImage(tiptext, true, _emotInfo->image);
  } else {
    tip = new Stamina::UI::ToolTipX::TipTextRich(tiptext);
  }

  _parent->_tooltip->setPos(pos, true, Stamina::UI::ToolTipX::enPositioning::positionFirst, Stamina::UI::ToolTipX::enPlacement::pRightBottom);
  _parent->_tooltip->setTip(tip.get(), false);
  _parent->_tooltip->show();
}

bool EmotPackInfoLV::EmotPackInfoItem::onMouseUp(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  if (_parent->draged) {
  Point p = pos;
  p.y-= _parent->getLV()->getScrollPos().y;
    UINT id = _parent->_lv->getItemIndex(li);
    Rect rc = lv->itemToClient(li->getRect());
    if (_parent->draged_id != id) {
      if (p.y < rc.getCenter().y) {
        if (_parent->draged_id != id) {
          _parent->moveItem(_parent->draged_id, id);
          _parent->_items[id - (id> _parent->draged_id ? 1 : 0)]->setSelected(_parent->getLV(), true);
        }
      } else {
        if (_parent->draged_id != id + 1) {
          _parent->moveItem(_parent->draged_id, id + 1);
          _parent->_items[id + (id< _parent->draged_id ? 1 : 0)]->setSelected(_parent->getLV(), true);
        }
      }
    }
    _parent->draged = false;
  }
  ReleaseCapture();
  return 1;
}

bool EmotPackInfoLV::EmotPackInfoItem::onMouseDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  if (_parent->_tooltip->visible()) _parent->_tooltip->hide();

  Point p = pos;
  p.x-= _parent->getLV()->getScrollPos().x;
  p.y-= _parent->getLV()->getScrollPos().y;

  Rect rc = lv->itemToClient(li->getRect());

  Rect rcinform = rc;
  rcinform.left = rcinform.right - 3 - 16;
  rcinform.top+=3;
  _inform->setPos(rcinform.getLT());

  Rect rccheck = rc;
  rccheck.left+=3;
  rccheck.top+=3;
  _check->setPos(rccheck.getLT());

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
  } else if (_inform->hitTest(p)) {
      showToolTip(_inform->getPos());
      return 0;
  } else {
    _parent->draged = true;
    _parent->draged_id = _parent->_lv->getItemIndex(li);
    SetCapture(_parent->_lv->getHwnd());
  }
  return 1;
}

bool EmotPackInfoLV::EmotPackInfoItem::onKeyDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, int info) {
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

  Rect rctxt = rc;
  rctxt.left+= 21;
  rctxt.top+= 3;
  rctxt.right-= 3 + 16 + 5;
  SetBkMode(dc, TRANSPARENT);

  Stamina::Point p = Point(0,0);

  Rect rcinform = rc;
  rcinform.left = rcinform.right - 3 - 16;
  rcinform.top+=3;
  _inform->setPos(rcinform.getLT());
  _inform->draw(dc, p);

  Rect rccheck = rc;
  rccheck.left+=3;
  rccheck.top+=3;
  _check->setPos(rccheck.getLT());
  _check->draw(dc, p);

  HFONT hOldFont = (HFONT)SelectObject(dc, _parent->hFont);
  DrawTextA(dc, name.c_str(), -1, rctxt.ref(), DT_NOPREFIX | DT_END_ELLIPSIS | DT_SINGLELINE);
  SelectObject(dc, hOldFont);

  lv->releaseDC(dc);
}