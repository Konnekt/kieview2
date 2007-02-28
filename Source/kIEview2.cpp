/**
  *  kIEview2 main file
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2006-2007 Sijawusz Pur Rahnama
  *  @copyright    Copyright (c) 2004-2005 Kuba 'nix' Niegowski
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision$
  *  @modifiedby   $LastChangedBy$
  *  @lastmodified $Date$
  */

#include "stdafx.h"
#include "Controller.h"

using namespace kIEview2;

int __stdcall DllMain(void* hinstDLL, unsigned long fdwReason, void* lpvReserved) {
  return true;
}

int __stdcall IMessageProc(sIMessage_base* msgBase) {
  return Controller::getInstance()->process(msgBase);
}