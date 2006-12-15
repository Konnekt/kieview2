/**
  *  kIEview2 PopupListner class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2004-2005 Kuba 'nix' Niegowski
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 16 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-08 19:37:43 +0100 (Pt, 08 gru 2006) $
  */

#pragma once

#ifndef __POPUPLISTNER_H__
#define __POPUPLISTNER_H__

#include "stdafx.h"
#include "IECtrl.h"

namespace kIEview2 {
  class PopupListner : public IECtrl::PopupMenuListener {
  public:
    MakeAction PopupMenu(MenuType type, POINT pt, IECtrl* ctrl);
  };
}

#endif // __POPUPLISTNER_H__