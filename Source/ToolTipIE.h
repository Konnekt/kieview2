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

#pragma once

#ifndef __TOOLTIP_IE_H__
#define __TOOLTIP_IE_H__

#include "IECtrl.h"
#include "Stamina/UI/ToolTipX.h"

using namespace Stamina::UI;
using namespace ToolTipX;

class ToolTipIE: public TipImpl {
public:
  STAMINA_OBJECT_CLASS(ToolTipIE, iTip);

public:
  ToolTipIE(int width, int height);
  ~ToolTipIE();

public:
  void adjustSize(HDC hdc, Rect& rc);
  void paint(HDC hdc, const Rect& rc);
  void detach(HWND hwnd);
  void attach(HWND hwnd);
  void setSize(int width, int height);

  IECtrl* getIECtrl() {
    return _ctrl;
  }

protected:
  IECtrl* _ctrl;

  int _width;
  int _height;
  bool _positioned;
};

#endif // __TOOLTIP_IE_H__