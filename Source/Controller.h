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
#include "PlugController.h"
#include "Helpers.h"
#include "IECtrl.h"
#include "ActionsHandler.h"
#include "TplHandler.h"
#include "TplUdf.h"

using namespace kIEview2;

namespace kIEview2 {
  class Controller : public PlugController<Controller> {
  public:
    friend class PlugController<Controller>;

  public:
    /* Class version */
    STAMINA_OBJECT_CLASS_VERSION(Controller, PlugController, Version(0,1,0,0));

  protected:
    Controller();

  public:
    ~Controller();

    String getStatusLabel(int status);
    string getMsgLabel(int type);

  protected:
    void _onPrepare();
    void _onAction();

    void _msgCtrlView();
    void _msgCtrlSend();
    void _msgSend();

    String _parseStatusTpl(int status, const char* info);
    String _parseMsgTpl(cMessage* msg);

  public:
    void clearWnd(IECtrl* ctrl);

  protected:
    ActionsHandler* actionsHandler;
    TplHandler* tplHandler;
  };
}

#endif // __CONTROLLER_H__