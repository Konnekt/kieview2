/**
  *  kIEview2 Popup Listener class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2004-2005 Kuba 'nix' Niegowski
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision$
  *  @modifiedby   $LastChangedBy$
  *  @lastmodified $Date$
  */

#pragma once

#ifndef __POPUPLISTENER_H__
#define __POPUPLISTENER_H__

#include "IECtrl.h"
#include "kIEView2.h"

using namespace kIEview2;

namespace kIEview2 {
  class PopupListener : public IECtrl::PopupMenuListener {
  public:
    MakeAction PopupMenu(MenuType type, POINT pt, IECtrl* ctrl);
  };
}

#endif // __POPUPLISTENER_H__