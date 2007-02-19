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

EmotLV::tEmotLVs EmotLV::_lvs;

EmotLV::EmotLV(int x, int y, int w, int h, HWND parent, HMENU id): ListWnd::ListView(x, y, w, h, parent, id), _enabled(true) {
  alwaysShowScrollbars(false, false);

  _lastProc = (WNDPROC)SetWindowLong(getHwnd(), GWL_WNDPROC, (LONG) eMsgProc);

  _checked = new Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::checked, IML_16), false);
  _unchecked = new Icon((HICON)ICMessage(IMI_ICONGET, kIEview2::ico::unchecked, IML_16), false);

  hFontBold = CreateFont(-11, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
    CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");
  hFont = CreateFont(-11, 0, 0, 0, FW_MEDIUM, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,
    CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");

  draged = false;
  draged_id = 0;
  mmitem = -1;

  _lvs.push_back(this);
}

LRESULT CALLBACK EmotLV::eMsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  EmotLV* elv = (EmotLV*)ListWnd::ListView::fromHWND(hwnd);
  switch(msg) {
    case WM_ENABLE:
      elv->_enabled = (bool)wParam;
      if (IsWindowVisible(hwnd)) {
        elv->repaintClient();
      }
    break;
    case WM_DESTROY:
      WNDPROC proc = elv->_lastProc;
      SetWindowLong(elv->getHwnd(), GWL_WNDPROC, (LONG) proc);
      return CallWindowProc(proc, hwnd, msg, wParam, lParam);
    break;
  }
  return CallWindowProc(elv->_lastProc, hwnd, msg, wParam, lParam);
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
  scrollToActive(false);
  draged = false;

  this->refreshItems();

  if (mmitem != -1) {
    _items[mmitem]->repaint(this);
    getRootItem()->setRefreshFlag(ListWnd::refreshSize, true);
  }
  mmitem = -1;
}

void EmotLV::onMouseDown(int vkey, const Stamina::Point &pos) {
  ListWnd::ListView::onMouseDown(vkey, pos);
  getRootItem()->setFlag(ListWnd::flagSubitemsChanged, true, this);
  refreshItems();
}

void EmotLV::onMouseMove(int vkey, const Stamina::Point &pos) {
  POINT pt;
  GetCursorPos(&pt);
  RECT rc;
  GetWindowRect(getHwnd(), &rc);

  if (rc.top > pt.y) {
    Point d = this->getScrollPos();
    d.y -= rc.top - pt.y;
    scrollTo(d);;
    Sleep(100);
  } else if (pos.y > getClientRect().bottom){
    Point d = this->getScrollPos();
    d.y += pos.y - getClientRect().bottom;
    scrollTo(d);
    Sleep(100);
  }
  ListWnd::ListView::onMouseMove(vkey, pos);
}

void EmotLV::onKeyDown(int vkey, int info) {
  ListWnd::ListView::onKeyDown(vkey, info);
  getRootItem()->setFlag(ListWnd::flagSubitemsChanged, true, this);
  refreshItems();
  scrollToActive(false);
}

EmotLV::~EmotLV() {
  removeAll();

  DeleteObject(hFontBold);
  DeleteObject(hFont);

  for (tEmotLVs::iterator it = _lvs.begin(); it != _lvs.end(); it++) {
    if ((*it) == this) {
      _lvs.erase(it); break;
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

  sEmotPackInfo* s = getEPI(pos);
  removeEntry(_items[pos]->getEntry());
  _items[pos] = insertEntry(new EmotPackInfoItem(this, s), pos).get();

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
  LockerCS locker(_lock);

  int addY = 0;
  if (_emotInfo->image.isValid()) {
    addY = _emotInfo->image->getSize().h;
  }
  if (addY < 16) addY = 16;
  if (li->isActive()) 
    return Size(fitIn.w, sizeInfo((EmotLV*)lv, Rect(0, 0, fitIn.w, 0)) + 6 + addY);
  return Size(fitIn.w, 6 + addY);
}

void EmotLV::EmotPackInfoItem::drawInfo(EmotLV* elv, Rect& rc) {
  eMSet* set = _emotInfo->set;
  HDC dc = elv->getDC();
  string textA = getItemText();
  RECT rcz = *rc.ref();

  HFONT hOldFont = (HFONT)SelectObject(dc, elv->hFont);
  DrawText(dc, textA.c_str(), -1, &rcz, DT_WORDBREAK);
  SelectObject(dc, hOldFont);
  elv->releaseDC(dc);
}

int EmotLV::EmotPackInfoItem::sizeInfo(EmotLV* elv, Rect& rc) {
  eMSet* set = _emotInfo->set;
  HDC dc = elv->getDC();
  string textA = getItemText();
  RECT rcz = {0, 0, rc.width(), 0};

  HFONT hOldFont = (HFONT)SelectObject(dc, elv->hFont);
  DrawText(dc, textA.c_str(), -1, &rcz, DT_CALCRECT | DT_WORDBREAK);
  SelectObject(dc, hOldFont);
  elv->releaseDC(dc);

  return rcz.bottom;
}

void EmotLV::EmotPackInfoItem::resizeItems(EmotLV* elv, ListWnd::Item* item) {
  ListWnd::oItem lastItem = elv->getActiveItem();
  elv->setActiveItem((ListWnd::Item*)item);
  if (lastItem.isValid()) {
    lastItem->setRefreshFlag(ListWnd::RefreshFlags::refreshAll);
  }
  item->setFlag(ListWnd::ItemFlags::flagResize, true);
  item->setFlag(ListWnd::ItemFlags::flagRepos, true);
  item->setRefreshFlag(ListWnd::RefreshFlags::refreshAll);
  elv->getRootItem()->setFlag(ListWnd::ItemFlags::flagSubitemsChanged, true);
  elv->getRootItem()->needResize();

  elv->refreshItems();
  elv->updateScrollbars();
}

bool EmotLV::EmotPackInfoItem::onMouseUp(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  LockerCS locker(_lock);

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
            elv->refreshItems();
            ListWnd::oItem item = elv->getEntryItem(elv->_items[id - (id > elv->draged_id ? 1 : 0)]->getEntry());
            resizeItems(elv, item.get());
            touchConfigWnd();
          }
          elv->mmitem = -1;
        }
      } else {
        if (elv->draged_id != id + 1) {
          if (elv->moveItem(elv->draged_id, id + 1)) {
            elv->refreshItems();
            ListWnd::oItem item = elv->getEntryItem(elv->_items[id + (id < elv->draged_id ? 1 : 0)]->getEntry());
            resizeItems(elv, item.get());
            touchConfigWnd();
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
  LockerCS locker(_lock);

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
      if (li->isActive()) {
        elv->draged = true;
        elv->draged_id = lv->getItemIndex(li);
        elv->mmitem = elv->draged_id;
      } else {
        resizeItems(elv, li.get());
        return false;
      }
      SetCapture(lv->getHwnd());

      return true;
    }
  }
  return true;
}

bool EmotLV::EmotPackInfoItem::onMouseMove(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  LockerCS locker(_lock);

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
      oImage gradient = createSimpleGradient(RGB(255, 204, 102), RGB(255, 153, 51), Size(rc.width(), 10));
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
  LockerCS locker(_lock);

  EmotLV* elv = (EmotLV*)lv;

  if (vkey == VK_LBUTTON) {
    switchState(lv);
    return false;
  }
  return true;
}

bool EmotLV::EmotPackInfoItem::onKeyDown(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, int info) {
  LockerCS locker(_lock);

  EmotLV* elv = (EmotLV*)lv;
  elv->getRootItem()->needResize();
  elv->refreshItems();

  if (vkey == VK_SPACE) {
    switchState(lv);
    return false;
  } else if (vkey == VK_UP){
    int id = lv->getItemIndex(li);
    if(--id >= 0) {
      resizeItems(elv, elv->_items[id]);
    }
  } else if (vkey == VK_DOWN){
    int id = lv->getItemIndex(li);
    if(++id < elv->itemsCount()) {
      resizeItems(elv, elv->_items[id]);
    }
  } else if (vkey == VK_HOME) {
    if (elv->itemsCount()) {
      resizeItems(elv, elv->_items[0]);
    }
  } else if (vkey == VK_END) {
    if (elv->itemsCount()) {
      resizeItems(elv, elv->_items[elv->itemsCount() - 1]);
    }
  }
  elv->getRootItem()->setFlag(ListWnd::flagSubitemsChanged, true, lv);
  elv->refreshItems();
  return false;
}

void EmotLV::EmotPackInfoItem::paintEntry(ListWnd::ListView* lv, const ListWnd::oItem& li, const ListWnd::oItemCollection& parent) {
  LockerCS locker(_lock);

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
  oImage gradient;

  SetTextColor(dc, RGB(255, 255, 255));
  COLORREF textColor;

  /* predefiniowane kolorki */
  if (elv->_enabled) {
    if (li->isActive()) {
      gradient = Stamina::UI::createSimpleGradient(RGB(0,0,0), RGB(60,60,60), Size(rc.width(), rc.height()));
      textColor = RGB(255,255,255);
    } else {
      if (_emotInfo->checked) {
        gradient = Stamina::UI::createSimpleGradient(RGB(190,252,122), RGB(219,253,181), Size(rc.width(), rc.height()));
      } else {
       gradient = Stamina::UI::createSimpleGradient(RGB(220,220,220), RGB(240,240,240), Size(rc.width(), rc.height()));
      }
      textColor = RGB(40,40,40);
    }
  } else {
      gradient = Stamina::UI::createSimpleGradient(0xC0C0C0, 0xC4C4C4, Size(rc.width(), rc.height()));
      textColor = 0xF0F0F0;
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

  int emotY = 0, emotX = 0;
  if (_emotInfo->image.isValid() && elv->_enabled) {
    Rect rcemot = rc;
    rcemot.left = rcemot.right - 3 - _emotInfo->image->getSize().w;
    rcemot.top += 3;
    _emotInfo->image->draw(dc, rcemot.getLT());
    emotY = _emotInfo->image->getSize().h;
    emotX = _emotInfo->image->getSize().w;
  }

  if (emotY < 16) emotY = 16;

  Rect rccheck = rc;
  rccheck.left += 3;
  rccheck.top += 3;
  _check->setPos(rccheck.getLT());
  _check->draw(dc, p);

  HFONT hOldFont = (HFONT)SelectObject(dc, elv->hFontBold);
  COLORREF oldTextColor = SetTextColor(dc, textColor);
  rctxt.right -= emotX;
  DrawTextA(dc, name.c_str(), -1, rctxt.ref(), DT_NOPREFIX | DT_END_ELLIPSIS | DT_SINGLELINE);
  rctxt.top += emotY;
  rctxt.right += emotX;
  SelectObject(dc, hOldFont);

  if (li->isActive()) {
    drawInfo(elv, rctxt);
  }
  SetTextColor(dc, oldTextColor);
  lv->releaseDC(dc);
}
