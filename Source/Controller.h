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
#include "CfgController.h"
#include "Helpers.h"
#include "IECtrl.h"

using namespace kIEview2;

namespace kIEview2 {
  class Controller : public IMController {
  public:
    /* Class version */
	  STAMINA_OBJECT_CLASS_VERSION(Controller, IMController, Version(0,1,0,0));

  protected:
    Controller();

  public:
    ~Controller();

    inline static Controller* getInstance() {
      if (!instance.isValid()) {
        instance = new Controller;
      }
      return instance;
    }

  protected:
    void _onPrepare();

    void _msgCtrlView();
    void _msgCtrlSend();
    void _msgSend();

  protected:
    static SharedPtr<Controller> instance;
    oCfgCtrl config;
  };
}

#endif // __CONTROLLER_H__