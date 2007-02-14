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
#include "emotUI.h"
#include "Emots.h"
#include "Helpers.h"

EmotLV::tEmotLVs EmotLV::_lvs;

EmotLV::EmotLV(int x, int y, int w, int h, HWND parent, HMENU id): ListWnd::ListView(x, y, w, h, parent, id) {
  alwaysShowScrollbars(false, false);

  _checked = new Stamina::UI::Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::checked, IML_16), false);
  _unchecked = new Stamina::UI::Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::unchecked, IML_16), false);

  hFont = CreateFont(-11, 0, 0, 0, FW_MEDIUM, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
    CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");

  draged = false;
  draged_id = 0;
  mmitem = -1;

  _lvs.push_back(this);
}

bool EmotLV::isVaildLV(EmotLV* lv) {
  for (tEmotLVs::iterator it = _lvs.begin(); it != _lvs.end(); it++) {
    if ((*it) == lv) return true;
  }
  return false;
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
    d.y -= rc.top - pt.y;
    scrollTo(d);
    Sleep(100);
  } else if (pos.y > getClientRect().bottom){
    Point d = this->getScrollPos();
    d.y += pos.y - getClientRect().bottom;
    scrollTo(d);
    Sleep(100);
  }
  ListWnd::ListView::onMouseMove(vkey, pos);
}

EmotLV::~EmotLV() {
  removeAll();
  DeleteObject(hFont);

  for (tEmotLVs::iterator it = _lvs.begin(); it != _lvs.end(); it++) {
    if ((*it) == this) {
      _lvs.erase(it);
      break;
    }
  }
}

void EmotLV::setPos(int x, int y) {
  MoveWindow(getHwnd(), x, y, 0, 0, SWP_NOSIZE);
}

void EmotLV::setSize(int w, int h) {
  MoveWindow(getHwnd(), 0, 0, w, h, SWP_NOMOVE);
}

bool EmotLV::isEnabled() {
  return IsWindowEnabled(getHwnd());
}

void EmotLV::setEnabled(bool enabled) {
  EnableWindow(getHwnd(), enabled);
}

UINT EmotLV::addItem(sEmotPackInfo *s) {
  _items.push_back(insertEntry(new EmotPackInfoItem(this, s)).get());
  getEPI(_items.size() - 1)->id = _items.size() - 1;
  return _items.size() - 1;
}

bool EmotLV::moveItem(UINT id, int pos) {
  if (id == -1 || pos == -1 || id == pos || id >= _items.size() || pos > _items.size()) return false;

  int inc = 0;
  tItems::iterator it = _items.begin();

  if (pos == 0) {
    _items.insert(it, _items[id]);
    it = _items.begin();
    inc = 0;
    while (inc++ < id + 1) it++;
    _items.erase(it);

  } else if (pos == _items.size()) {
    _items.push_back(_items[id]);
    it = _items.begin();
    while (inc++ < id) it++;
    _items.erase(it);
    pos -= 1;

  } else if ((id <= _items.size() - 1) && (pos <= _items.size() - 1)) {
    if (id >= pos) {
      while (inc++ < pos) it++;
      _items.insert(it, _items[id]);

      it = _items.begin();
      inc = 0;
      while (inc++ < id + 1) it++;
      _items.erase(it);
    } else {
      while (inc++ < pos) it++;
      _items.insert(it, _items[id]);

      it = _items.begin();
      inc = 0;
      while (inc++ < id) it++;
      _items.erase(it);
      pos -=1;
    }
  } else {
    return false;
  }

  ListWnd::Item* item = _items[pos];
  ListWnd::oEntry entry = item->getEntry();
  removeEntry(entry, false);
  _items[pos] = insertEntry(entry, pos).get();

  for (int i = 0; i < itemsCount(); i++) {
    getEPI(i)->id = i;
  }

  return true;
}

void EmotLV::removeItem(UINT id) {
  removeEntry(_items[id]->getEntry());
  getEPI(id)->id = -1;
  tItems::iterator it = _items.begin() + id;
  _items.erase(it);
}

void EmotLV::removeAllItems() {
  for (int i = 0; i < itemsCount(); i++) {
    getEPI(i)->id = -1;
  }
  removeAll();
  _items.clear();
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
  int addY = 0;
  if (_emotInfo->image.isValid()) {
    addY = _emotInfo->image->getSize().h;
  }
  if (addY < 16) addY = 16;
  if (li->isSelected()) return Size(fitIn.w, sizeInfo((EmotLV*)lv, Rect(0, 0, fitIn.w, 0)) + 6 + addY);
  return Size(fitIn.w, 6 + addY);
}

void EmotLV::EmotPackInfoItem::drawInfo(EmotLV* elv, Rect& rc) {
  eMSet* set = _emotInfo->set;
  HDC dc = elv->getDC();
  string textA;

  RECT rcz = *rc.ref();
  if (set->getUrl().length()) textA += "URL: " + set->getUrl() + "\n";
  if (set->getDescription().length()) textA += "Opis: " + set->getDescription();
  textA = Helpers::rtrim(textA, "\n");
  HFONT hOldFont = (HFONT)SelectObject(dc, elv->hFont);
  DrawText(dc, textA.c_str(), -1, &rcz, DT_WORDBREAK);
  SelectObject(dc, hOldFont);
  elv->releaseDC(dc);
 }

int EmotLV::EmotPackInfoItem::sizeInfo(EmotLV* elv, Rect& rc) {
  eMSet* set = _emotInfo->set;
  HDC dc = elv->getDC();
  string textA;

  RECT rcz = {0, 0, rc.width(), 0};
  if (set->getUrl().length()) textA += "URL: " + set->getUrl() + "\n";
  if (set->getDescription().length()) textA += "Opis: " + set->getDescription();
  textA = Helpers::rtrim(textA, "\n");
  HFONT hOldFont = (HFONT)SelectObject(dc, elv->hFont);
  DrawText(dc, textA.c_str(), -1, &rcz, DT_CALCRECT | DT_WORDBREAK);
  SelectObject(dc, hOldFont);
  elv->releaseDC(dc);
  return rcz.bottom;
}

bool EmotLV::EmotPackInfoItem::onMouseUp(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  EmotLV* elv = (EmotLV*)lv;

  if (elv->draged) {
    if (elv->mmitem != -1) {
      elv->_items[elv->mmitem]->repaint(lv);
    }

    Point p = pos;
    p.y -= elv->getScrollPos().y;
    UINT id = elv->getItemIndex(li);
    Rect rc = lv->itemToClient(li->getRect());

    if (elv->draged_id != id) {
      if (p.y < rc.getCenter().y) {
        if (elv->draged_id != id) {
          if (elv->moveItem(elv->draged_id, id)) {
            elv->_items[id - (id > elv->draged_id ? 1 : 0)]->setSelected(elv, true);
            SendMessage((HWND)UIGroupHandle(sUIAction(0, IMIG_CFGWND)), WM_USER + 18091, 0, 0);
          }
          elv->mmitem = -1;
        }
      } else {
        if (elv->draged_id != id + 1) {
          if (elv->moveItem(elv->draged_id, id + 1)) {
            elv->_items[id + (id < elv->draged_id ? 1 : 0)]->setSelected(elv, true);
            SendMessage((HWND)UIGroupHandle(sUIAction(0, IMIG_CFGWND)), WM_USER + 18091, 0, 0);
          }
          elv->mmitem = -1;
        }
      }
    }
    elv->draged = false;
  }
  ReleaseCapture();
  return true;
}

bool EmotLV::EmotPackInfoItem::onMouseDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  EmotLV* elv = (EmotLV*)lv;

  if (vkey == VK_LBUTTON) {
    Point p = pos;
    p.x -= elv->getScrollPos().x;
    p.y -= elv->getScrollPos().y;

    Rect rc = lv->itemToClient(li->getRect());

    Rect rccheck = rc;
    rccheck.left += 3;
    rccheck.top += 3;
    _check->setPos(rccheck.getLT());

    if (_check->hitTest(p)) {
      switchState(lv);
      return false;
    } else {
      elv->draged = true;
      elv->draged_id = lv->getItemIndex(li);
      elv->mmitem = elv->draged_id;
      ListWnd::oItem lastItem = lv->getActiveItem();
      if (lastItem.isValid()) {
        lastItem->setRefreshFlag(ListWnd::RefreshFlags::refreshAll);
      }
      lv->setActiveItem(li);
      lv->selectionToActive();
      li->setSelected(lv, true);
      li->setRefreshFlag(ListWnd::RefreshFlags::refreshAll);
      SetCapture(lv->getHwnd());
      return false;
    }
  }
  return true;
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
    p.y -= lv->getScrollPos().y;

    if (elv->draged_id != id) {
      Stamina::UI::oImage gradient = Stamina::UI::createSimpleGradient(RGB(255, 204, 102), RGB(255, 153, 51), Size(rc.width(), 10));
      if (p.y < rc.getCenter().y) {
        if (elv->draged_id != id) {
          Rect rct = rc;
          rct.top += rc.height() / 2;
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
          rct.bottom -= rc.height() / 2;
          lv->repaintRect(rct);
          gradient->draw(dc, Point(rc.left, rc.bottom - 10));
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
    return false;
  }
  return true;
}

bool  EmotLV::EmotPackInfoItem::onMouseDblClk(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  EmotLV* elv = (EmotLV*)lv;

  if (vkey == VK_LBUTTON) {
    switchState(lv);
    return false;
  }
  return true;
}

bool EmotLV::EmotPackInfoItem::onKeyDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, int info) {
  EmotLV* elv = (EmotLV*)lv;

  if (vkey == VK_SPACE) {
    switchState(lv);
  } else if (vkey == VK_UP){
    int id = lv->getItemIndex(li);
    if(--id >= 0) {
      li->setRefreshFlag(ListWnd::RefreshFlags::refreshAll);
      lv->setActiveItem(elv->_items[id]);
      lv->selectionToActive();
      elv->_items[id]->setSelected(lv, true);
      elv->_items[id]->setRefreshFlag(ListWnd::RefreshFlags::refreshAll);
      lv->scrollToActive();
    }
  } else if (vkey == VK_DOWN){
    int id = lv->getItemIndex(li);
    if(++id < elv->itemsCount()) {
      li->setRefreshFlag(ListWnd::RefreshFlags::refreshAll);
      lv->setActiveItem(elv->_items[id]);
      lv->selectionToActive();
      elv->_items[id]->setSelected(lv, true);
      elv->_items[id]->setRefreshFlag(ListWnd::RefreshFlags::refreshAll);
      lv->scrollToActive();
    }
  } else if (vkey == VK_HOME) {
    if (elv->itemsCount()) {
      li->setRefreshFlag(ListWnd::RefreshFlags::refreshAll);
      lv->setActiveItem(elv->_items[0]);
      lv->selectionToActive();
      elv->_items[0]->setSelected(lv, true);
      elv->_items[0]->setRefreshFlag(ListWnd::RefreshFlags::refreshAll);
      lv->scrollToActive();
    }
  } else if (vkey == VK_END) {
    if (elv->itemsCount()) {
      li->setRefreshFlag(ListWnd::RefreshFlags::refreshAll);
      lv->setActiveItem(elv->_items[elv->itemsCount() - 1]);
      lv->selectionToActive();
      elv->_items[elv->itemsCount() - 1]->setSelected(lv, true);
      elv->_items[elv->itemsCount() - 1]->setRefreshFlag(ListWnd::RefreshFlags::refreshAll);
      lv->scrollToActive();
    }
  }
  return false;
}

void EmotLV::EmotPackInfoItem::paintEntry(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent) {
  EmotLV* elv = (EmotLV*)lv;
  HDC dc = lv->getDC();
  Rect rc = lv->itemToClient(li->getRect());

  string name = _emotInfo->set->getName();
  if (_emotInfo->set->getVersion().length()) {
    name += " (" + _emotInfo->set->getVersion() + ")";
  }

  Rect rci = rc;
  rci.left += 25;
  rci.top += 2;
  Stamina::UI::oImage gradient;

  SetTextColor(dc, RGB(255, 255, 255));
  COLORREF textColor;

  if (li->isSelected()) {
    gradient = Stamina::UI::createSimpleGradient(Stamina::UI::blendRGB(GetSysColor(COLOR_HIGHLIGHT), GetSysColor(COLOR_WINDOW), 0xD0), 
      GetSysColor(COLOR_HIGHLIGHT), Size(rc.width(), rc.height()));
    textColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
  } else {
    if (_emotInfo->checked) {
      gradient = Stamina::UI::createSimpleGradient(Stamina::UI::blendRGB(GetSysColor(COLOR_HIGHLIGHT), GetSysColor(COLOR_WINDOW), 0x60),
      Stamina::UI::blendRGB(GetSysColor(COLOR_HIGHLIGHT), GetSysColor(COLOR_WINDOW), 0x80), Size(rc.width(), rc.height()));
      textColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
    } else {
      gradient = Stamina::UI::createSimpleGradient(GetSysColor(COLOR_BTNFACE),
        Stamina::UI::blendRGB(GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_BTNFACE), 0x10),
      Size(rc.width(), rc.height()));
      textColor = GetSysColor(COLOR_BTNTEXT);
    }
  }
  gradient->draw(dc, rc.getLT());

  Rect rctxt = rc;
  rctxt.left += 21;
  rctxt.top += 3;
  rctxt.right -= 3 + 16 + 5;
  SetBkMode(dc, TRANSPARENT);

  Stamina::Point p = Point(0,0);

  int emotY = 0, emotX = 0;
  if (_emotInfo->image.isValid()) {
    Rect rcemot = rc;
    rcemot.left = rcemot.right - 3 - _emotInfo->image->getSize().w;
    rcemot.top += 3;
    _emotInfo->image->draw(dc, rcemot.getLT());
    emotY = _emotInfo->image->getSize().h;
    emotX = _emotInfo->image->getSize().w;
  }

  Rect rccheck = rc;
  rccheck.left += 3;
  rccheck.top += 3;
  _check->setPos(rccheck.getLT());
  _check->draw(dc, p);

  HFONT hOldFont = (HFONT)SelectObject(dc, elv->hFont);
  COLORREF oldTextColor = SetTextColor(dc, textColor);
  rctxt.right-= emotX;
  DrawTextA(dc, name.c_str(), -1, rctxt.ref(), DT_NOPREFIX | DT_END_ELLIPSIS | DT_SINGLELINE);
  rctxt.top+= emotY;
  rctxt.right+= emotX;
  SelectObject(dc, hOldFont);
  if (li->isSelected()) {
    drawInfo(elv, rctxt);
  }
  SetTextColor(dc, oldTextColor);
  lv->releaseDC(dc);
}
