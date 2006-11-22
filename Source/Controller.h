/**
  *  kIEview2 Control class
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

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "kIEview2.h"
#include "IMController.h"
#include "Helpers.h"
#include "IECtrl.h"

using namespace kIEview2;

namespace kIEview2 {
  class Controller : public IMController<Controller>, signals::trackable {
  public:
    friend class IMController<Controller>;

  protected:
    Controller();
    ~Controller();

  protected:
    void _onPrepare();

    void _msgCtrlView();
    void _msgCtrlSend();
    void _msgSend();

  protected:
    int ctrlSendActionOwner;
  };
}

#endif // __CONTROLLER_H__