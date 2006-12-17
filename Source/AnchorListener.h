/**
  *  kIEview2 Anchor Listener class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2004-2005 Kuba 'nix' Niegowski
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 25 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-12-16 15:46:15 +0100 (So, 16 gru 2006) $
  */

#pragma once

#ifndef __ANCHORLISTENER_H__
#define __ANCHORLISTENER_H__

#include "IECtrl.h"
#include "kIEView2.h"

using namespace kIEview2;

namespace kIEview2 {
  class AnchorListener : public IECtrl::AnchorClickListener {
  public:
    void AnchorClicked(const char* url, IECtrl* ctrl);
  };
}

#endif // __ANCHORLISTENER_H__