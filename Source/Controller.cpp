/**
  *  kIEview2 Controller class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2005-2006 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 77 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-11-20 17:42:35 +0100 (Pn, 20 lis 2006) $
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