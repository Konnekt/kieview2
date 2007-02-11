#include "stdafx.h"
#include "emotUI.h"

EmotLV::EmotLV(int x, int y, int w, int h, HWND parent, HMENU id): ListWnd::ListView(x, y, w, h, parent, id) {
  alwaysShowScrollbars(false, true);

  _checked = new Stamina::UI::Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::checked, IML_16), false);
  _unchecked = new Stamina::UI::Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::unchecked, IML_16), false);
  _inform = new Stamina::UI::Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::emotsinfo, IML_16), false);

  hFont = CreateFont(16, 0, 0, 0, FW_MEDIUM, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
      CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Verdana");

  _tooltip = new Stamina::UI::ToolTipX::ToolTip(getHwnd(), 0);
  Stamina::UI::ToolTipX::Tip* tip = new Stamina::UI::ToolTipX::Tip();
  _tooltip->setTip(tip, false);

  draged = false;
  draged_id = 0;
  mmitem = -1;
}


void EmotLV::onMouseUp(int vkey, const Stamina::Point &pos) {
  ReleaseCapture();
  ListWnd::ListView::onMouseUp(vkey, pos);
  draged = false;
  if (mmitem != -1) {
    _items[mmitem]->repaint(this);
  }
  mmitem = -1;
}

void EmotLV::onMouseMove(int vkey, const Stamina::Point &pos) {
  POINT pt;
  GetCursorPos(&pt);
  RECT rc;
  GetWindowRect(getHwnd(), &rc);
  if (rc.top > pt.y) {
    Point d = this->getScrollPos();
    d.y-= rc.top - pt.y;
    scrollTo(d);
    Sleep(100);
  } else if (pos.y > getClientRect().bottom){
    Point d = this->getScrollPos();
    d.y+= pos.y - getClientRect().bottom;
    scrollTo(d);
    Sleep(100);
  }
  ListWnd::ListView::onMouseMove(vkey, pos);
}

EmotLV::~EmotLV() {
  removeAll();
  DeleteObject(hFont);
  delete _tooltip;
}

void EmotLV::setPos(int x, int y) {
  MoveWindow(getHwnd(), x, y, 0, 0, SWP_NOSIZE);
}

void EmotLV::setSize(int w, int h) {
  MoveWindow(getHwnd(), 0, 0, w, h, SWP_NOMOVE);
}

UINT EmotLV::addItem(sEmotPackInfo *s) {
  _items.push_back(insertEntry(new EmotPackInfoItem(this, s)).get());
  return _items.size() - 1;
}

bool EmotLV::moveItem(UINT id, int pos) {
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
  removeEntry(entry, false);
  _items[pos] = insertEntry(entry, pos).get();

  return true;
}

int EmotLV::removeItem(UINT id) {
  removeEntry(_items[id]->getEntry());
  tItems::iterator it = _items.begin() + id;
  _items.erase(it);
  return id;
}

int EmotLV::itemsCount() {
  return _items.size();
}

ListWnd::oItem EmotLV::getItem(UINT id) {
  if (id > _items.size()) return NULL;
  return _items[id];
}

EmotLV::sEmotPackInfo* EmotLV::getEPI(UINT id) {
  if (id > _items.size()) return NULL;
  return ((EmotPackInfoItem*)_items[id]->getEntry().get())->getEmotPackInfo();
}

EmotLV::sEmotPackInfo* EmotLV::EmotPackInfoItem::getEmotPackInfo() {
  return _emotInfo;
}

Size EmotLV::EmotPackInfoItem::getMinSize() {
  return Size(30, 32);
}

Size EmotLV::EmotPackInfoItem::getMaxSize() {
  return Size(0x7FFF, 32);
}

Size EmotLV::EmotPackInfoItem::getQuickSize() {
  return Size(0, 32);
}

Size EmotLV::EmotPackInfoItem::getEntrySize(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent, Size fitIn) {
  return Size(fitIn.w, 22);
}
void EmotLV::EmotPackInfoItem::showToolTip(EmotLV* elv, Point& pos) {
  string tiptext = stringf("Zestaw emot: <b>%s</b>", _emotInfo->name.c_str());
  if (_emotInfo->authors.length())
    tiptext+= stringf("<br/>Autorzy: <b>%s</b>", _emotInfo->authors.c_str());
  if (_emotInfo->releasedTime.length())
    tiptext+= stringf("<br/>Stworzono: <b>%s</b>", _emotInfo->releasedTime.c_str());
  if (_emotInfo->other.length()) 
    tiptext+= stringf("<br/>Dodatkowe: %s", _emotInfo->other.c_str());

  Stamina::UI::ToolTipX::oTip tip;
  if (_emotInfo->image.isValid()) {
    tip = Stamina::UI::ToolTipX::Tip::textAndImage(tiptext, true, _emotInfo->image);
  } else {
    tip = new Stamina::UI::ToolTipX::TipTextRich(tiptext);
  }

  elv->_tooltip->setPos(pos, true, Stamina::UI::ToolTipX::enPositioning::positionFirst, Stamina::UI::ToolTipX::enPlacement::pRightBottom);
  elv->_tooltip->setTip(tip.get(), false);
  elv->_tooltip->show();
}

bool EmotLV::EmotPackInfoItem::onMouseUp(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  EmotLV* elv = (EmotLV*)lv;
  if (elv->draged) {
    if (elv->mmitem != -1) {
      elv->_items[elv->mmitem]->repaint(lv);
    }
    Point p = pos;
    p.y-= elv->getScrollPos().y;
    UINT id = elv->getItemIndex(li);
    Rect rc = lv->itemToClient(li->getRect());
    if (elv->draged_id != id) {
      if (p.y < rc.getCenter().y) {
        if (elv->draged_id != id) {
          elv->moveItem(elv->draged_id, id);
          elv->_items[id - (id > elv->draged_id ? 1 : 0)]->setSelected(elv, true);
          elv->mmitem = -1;
        }
      } else {
        if (elv->draged_id != id + 1) {
          elv->moveItem(elv->draged_id, id + 1);
          elv->_items[id + (id < elv->draged_id ? 1 : 0)]->setSelected(elv, true);
          elv->mmitem = -1;
        }
      }
    }
    elv->draged = false;
  }
  ReleaseCapture();
  return 1;
}

bool EmotLV::EmotPackInfoItem::onMouseDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  EmotLV* elv = (EmotLV*)lv;
  if (vkey == VK_LBUTTON) {
    if (elv->_tooltip->visible()) elv->_tooltip->hide();

    Point p = pos;
    p.x-= elv->getScrollPos().x;
    p.y-= elv->getScrollPos().y;

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
        _check->setImage(elv->_unchecked.get());
        this->_emotInfo->checked = false;
        this->refreshEntry(lv, Stamina::ListWnd::RefreshFlags::refreshPaint);
      } else {
        _check->setImage(elv->_checked.get());
        this->_emotInfo->checked = true;
        this->refreshEntry(lv, Stamina::ListWnd::RefreshFlags::refreshPaint);
      }
      return 0;
    } else if (_inform->hitTest(p)) {
        showToolTip(static_cast<EmotLV*>(lv), _inform->getPos());
        return 0;
    } else {
      elv->draged = true;
      elv->draged_id = lv->getItemIndex(li);
      elv->mmitem = elv->draged_id;
      SetCapture(lv->getHwnd());
    }
  }
  return 1;
}

bool EmotLV::EmotPackInfoItem::onMouseMove(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  EmotLV* elv = (EmotLV*)lv;
  if (elv->draged) {
    int id = lv->getItemIndex(li);
    if (elv->mmitem != -1) {
      if (elv->mmitem != id) {
        elv->_items[elv->mmitem]->repaint(lv);
        elv->mmitem = id;
      }
    }
    HDC dc = lv->getDC();
    Rect rc = lv->itemToClient(li->getRect());
    Point p = pos;
    p.y-= lv->getScrollPos().y;
    if (elv->draged_id != id) {
      Stamina::UI::oImage gradient = Stamina::UI::createSimpleGradient(RGB(255, 204, 102), RGB(255, 153, 51), Size(rc.width(),rc.height() - 13));
      if (p.y < rc.getCenter().y) {
        if (elv->draged_id != id) {
          Rect rct = rc;
          rct.top+= rc.height() / 2;
          lv->repaintRect(rct);
          gradient->draw(dc, Point(rc.left, rc.top));
          HPEN hOldPen = (HPEN)SelectObject(dc, CreatePen(PS_SOLID, 1, RGB(0, 153, 0)));
          HBRUSH hOldBrush = (HBRUSH)SelectObject(dc, CreateSolidBrush(RGB(204, 255, 204)));
          POINT p[] = {{rc.getCenter().x - 3, rc.top + 7}, {rc.getCenter().x, rc.top + 1}, {rc.getCenter().x + 3, rc.top + 7}};
          Polygon(dc, p, 3);
          DeleteObject(SelectObject(dc, hOldPen));
          DeleteObject(SelectObject(dc, hOldBrush));
        }
      } else {
        if (elv->draged_id != id + 1) {
          Rect rct = rc;
          rct.bottom-= rc.height() / 2;
          lv->repaintRect(rct);
          gradient->draw(dc, Point(rc.left, rc.bottom - (rc.getHeight() - 13)));
          POINT p[] = {{rc.getCenter().x - 3, rc.bottom - 8}, {rc.getCenter().x, rc.bottom - 2}, {rc.getCenter().x + 3, rc.bottom - 8}};
          HPEN hOldPen = (HPEN)SelectObject(dc, CreatePen(PS_SOLID, 1, RGB(0, 153, 0)));
          HBRUSH hOldBrush = (HBRUSH)SelectObject(dc, CreateSolidBrush(RGB(204, 255, 204)));
          Polygon(dc, p, 3);
          DeleteObject(SelectObject(dc, hOldPen));
          DeleteObject(SelectObject(dc, hOldBrush));
        }
      }
    }
    lv->releaseDC(dc);
    return 0;
  }
  return 1;
}

bool EmotLV::EmotPackInfoItem::onKeyDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, int info) {
  EmotLV* elv = (EmotLV*)lv;
  if (vkey == VK_SPACE) {
    if (_emotInfo->checked) {
      _check->setImage(elv->_unchecked.get());
      _emotInfo->checked = false;
      refreshEntry(lv, Stamina::ListWnd::RefreshFlags::refreshPaint);
    } else {
      _check->setImage(elv->_checked.get());
      _emotInfo->checked = true;
      refreshEntry(lv, Stamina::ListWnd::RefreshFlags::refreshPaint);
    }
    return 0;
  } else {
    return 1;
  }
}

void EmotLV::EmotPackInfoItem::paintEntry(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent) {
  EmotLV* elv = (EmotLV*)lv;
  HDC dc = lv->getDC();
  Rect rc = lv->itemToClient(li->getRect());
  string name = _emotInfo->name;

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

  HFONT hOldFont = (HFONT)SelectObject(dc, elv->hFont);
  DrawTextA(dc, name.c_str(), -1, rctxt.ref(), DT_NOPREFIX | DT_END_ELLIPSIS | DT_SINGLELINE);
  SelectObject(dc, hOldFont);

  lv->releaseDC(dc);
}