/**
  *  kIEview2 Drop Listener class
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

#ifndef __DROPLISTENER_H__
#define __DROPLISTENER_H__

#include "IECtrl.h"
#include "kIEView2.h"

using namespace kIEview2;

namespace kIEview2 {
  class DropListener : public IECtrl::DropListener {
  public:
    void FileDropped(const char *url, IECtrl* ctrl);
  };
}

#endif // __DROPLISTENER_H__