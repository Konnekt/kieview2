/**
  *  kIEview2 Control class
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2005-2006 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 73 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-11-18 15:53:42 +0100 (So, 18 lis 2006) $
  */

#pragma once

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "kIEview2.h"
#include "IMController.h"

namespace kIEview2 {
  class Controller : public IMController<Controller>, signals::trackable {
  public:
    friend class IMController<Controller>;

  protected:
    Controller();
  };
}

#endif // __CONTROLLER_H__