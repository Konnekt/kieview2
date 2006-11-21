/**
  *  kIEview2 Controller class
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

#include "stdafx.h"
#include "Controller.h"

namespace kIEview2 {
  Controller::Controller() {
    /* Static values like net, type or version */
    this->addStaticValue(IM_PLUG_TYPE, IMT_CONFIG | IMT_MSGUI | IMT_UI);
    this->addStaticValue(IM_PLUG_PRIORITY, PLUGP_HIGH + 1);
    this->addStaticValue(IM_PLUG_NAME, (int) "kIEview2");
    this->addStaticValue(IM_PLUG_SIG, (int) sig);
    this->addStaticValue(IM_PLUG_NET, net);
  }
}