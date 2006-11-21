/**
  *  kIEview2 main file
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2004-2005 Kuba 'nix' Niegowski
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 66 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-11-14 10:43:57 +0100 (Wt, 14 lis 2006) $
  */

#include "stdafx.h"
#include "Controller.h"

using namespace kIEview2;

int __stdcall DllMain(void* hinstDLL, unsigned long fdwReason, void* lpvReserved) {
  return true;
}

int __stdcall IMessageProc(sIMessage_base* msgBase) {
  if (Controller::getInstance()->process(msgBase)) {
    return Controller::getInstance()->getReturnCode();
  } else {
    if (Ctrl) Ctrl->setError(IMERROR_NORESULT);
  }
  return 0;
}