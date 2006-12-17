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

#include "stdafx.h"
#include "Controller.h"
#include "IECtrl.h"
#include "AnchorListener.h"

namespace kIEview2 {
  void DropListener::FileDropped(const char *url, IECtrl* ctrl)
  {
    Ctrl->IMessage(im::fileDrop, NET_BROADCAST, -1, (int)url);
  }
}