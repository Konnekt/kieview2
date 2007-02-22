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

EmotLV::EmotLV(sUIActionNotify_createWindow* an, int w, int h): iLV(an, w, h) {
  draged = false;
  draged_id = 0;
  mmitem = -1;
}

void EmotLV::onMouseUp(int vkey, const Stamina::Point &pos) {
  ReleaseCapture();
  ListWnd::ListView::onMouseUp(vkey, pos);
  draged = false;
  if (mmitem != -1) {
    getItem(mmitem)->repaint(this);
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

UINT EmotLV::addItem(sEmotPackInfo *s) {
  ListWnd::oItem item = insertEntry(new EmotPackInfoItem(this, s));
  UINT id = getItemIndex(item);
  getEPI(getItemIndex(item))->id = id;

  if (getEPI(id)->checked) {
    selectItem(id, true);
  }
  return id;
}

EmotLV::sEmotPackInfo* EmotLV::getEPI(UINT id) {
  ListWnd::oItem item = this->getItem(id);

  if (item) {
    EmotPackInfoItem* ei = (EmotPackInfoItem*) item->getEntry().get();
    return ei->getEmotPackInfo();
  }
  return NULL;
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
  if (li->isActive()) {
    return Size(fitIn.w, sizeInfo((EmotLV*)lv, Rect(0, 0, fitIn.w, 0)) + 6 + addY);
  }
  return Size(fitIn.w, 6 + addY);
}

bool EmotLV::EmotPackInfoItem::onMouseUp(ListWnd::ListView* lv, const ListWnd::oItem& li, int level, int vkey, const Point& pos) {
  LockerCS locker(_lock);

  EmotLV* elv = (EmotLV*)lv;

  if (elv->draged) {
    if (elv->mmitem != -1) {
      elv->getItem(elv->mmitem)->repaint(lv);
    }

    Point p = pos;
    p.y -= elv->getScrollPos().y;
    UINT id = elv->getItemIndex(li);
    Rect rc = lv->itemToClient(li->getRect());

    if (elv->draged_id != id) {
      if (p.y < rc.getCenter().y) {
        if (elv->draged_id != id) {
          if (elv->getRootItem()->moveItem(elv, elv->getItem(elv->draged_id), elv->getItem(id), true)) {
            elv->refreshEPIID();
            touchConfigWnd();
          }
          elv->mmitem = -1;
        }
      } else {
        if (elv->draged_id != id + 1) {
          if (elv->getRootItem()->moveItem(elv, elv->getItem(elv->draged_id), elv->getItem(id + 1), true)) {
            elv->refreshEPIID();
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
    _selectBtn->setPos(rccheck.getLT());

    if (_selectBtn->hitTest(p)) {
      switchState(elv);
      return false;

    } else {
      if (li->isActive()) {
        elv->draged = true;
        elv->draged_id = lv->getItemIndex(li);
        elv->mmitem = elv->draged_id;
      }
      SetCapture(lv->getHwnd());
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
        elv->getItem(elv->mmitem)->repaint(lv);
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
  if (elv->isEnabled()) {
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
    if (isSelected()) {
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
  if (_emotInfo->image.isValid() && elv->isEnabled()) {
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
  _selectBtn->setPos(rccheck.getLT());
  _selectBtn->draw(dc, p);

  HFONT oldFont = (HFONT)SelectObject(dc, elv->_fontBold);
  COLORREF oldTextColor = SetTextColor(dc, textColor);
  rctxt.right -= emotX;
  DrawTextA(dc, name.c_str(), -1, rctxt.ref(), DT_NOPREFIX | DT_END_ELLIPSIS | DT_SINGLELINE);
  rctxt.top += emotY;
  rctxt.right += emotX;
  SelectObject(dc, oldFont);

  if (li->isActive()) {
    drawInfo(elv, rctxt);
  }
  SetTextColor(dc, oldTextColor);
  lv->releaseDC(dc);
}
