/**
  *  Plugin Controller class
  *
  *  Licensed under The GNU Lesser General Public License
  *  Redistributions of files must retain the above copyright notice.
  *
  *  @filesource
  *  @copyright    Copyright (c) 2005-2006 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kaway2/ kAway2 plugin SVN Repo
  *  @version      $Revision$
  *  @modifiedby   $LastChangedBy$
  *  @lastmodified $Date$
  *  @license      http://creativecommons.org/licenses/LGPL/2.1/
  */

#pragma once

#ifndef __PLUGCTRL_H__
#define __PLUGCTRL_H__

#include "IMController.h"
#include "CfgController.h"

using namespace Stamina;
using namespace boost;

namespace Konnekt {
  template <class Plug>
  class PlugController : public IMController {
  public:
    /* Class version */
    STAMINA_OBJECT_CLASS_VERSION(PlugController, IMController, Version(0,1,0,0));

  protected:
    inline PlugController() {
      config = new CfgController(this);
    }
    inline ~PlugController() {
      delete config;
    }

  public:
    inline static Plug* getInstance() {
      if (!instance.isValid()) {
        instance = new Plug;
      }
      return instance;
    }

    inline static CfgController* getConfig() {
      return getInstance()->config;
    }

  protected:
    static SharedPtr<Plug> instance;
    CfgController* config;
  };

  template <class Plug>
  SharedPtr<Plug> PlugController<Plug>::instance = 0;
}

#endif // __PLUGCTRL_H__