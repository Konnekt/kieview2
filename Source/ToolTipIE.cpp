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

ToolTipIE::ToolTipIE(int width, int height) {
  this->_ctrl = NULL;
  this->_positioned = false;
  this->_width = width;
  this->_height = height;
}

ToolTipIE::~ToolTipIE() {
  if (this->_ctrl) {
    delete this->_ctrl;
  }
}

void ToolTipIE::adjustSize(HDC hdc, Stamina::Rect& rc) {
  rc = Rect(0, 0, this->_width, this->_height);
}

void ToolTipIE::paint(HDC hdc, const Rect& rc) {
  if (!this->_positioned) {
    this->_positioned = true;
    SetWindowPos(_ctrl->getHWND(), 0, rc.left, rc.top, rc.width(), rc.height(), SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOZORDER);
  }
}

void ToolTipIE::attach(HWND hwnd) {
  if (!_ctrl) {
    this->_ctrl = new IECtrl(hwnd, 0, 0, this->_width, this->_height);
  }
}

void ToolTipIE::detach(HWND hwnd) {
  if (_ctrl) {
    delete _ctrl;
    this->_ctrl = NULL;
  }
}
void ToolTipIE::setSize(int width, int height) {
  this->_width = width;
  this->_height = height;
}