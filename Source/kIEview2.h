/**
  *  kIEview2 header file
  *
  *  Any modifications or reusing strictly prohibited!
  *
  *  @filesource
  *  @copyright    Copyright (c) 2005-2006 Sijawusz Pur Rahnama
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision: 76 $
  *  @modifiedby   $LastChangedBy: sija $
  *  @lastmodified $Date: 2006-11-20 07:32:01 +0100 (Pn, 20 lis 2006) $
  */

#pragma once

#ifndef __KIEVIEW2_H__
#define __KIEVIEW2_H__

namespace kIEview2 {
  const unsigned int net = 880;
  const char sig[] = "KIEV2";

  namespace api {
    const unsigned int api = net * 1000 + IM_USER;
  }

  namespace im {
    const unsigned int im = net * 1000 + IM_USER + 100;
  }

  namespace ui {
    const unsigned int ui = net * 1000;
  }

  namespace ico {
    const unsigned int ico = net * 1000 + 100;
  }

  namespace act {
    const unsigned int act = net * 1000 + 200;
  }

  // dynamiczne akcje
  namespace dynAct {
    const unsigned int dynAct = net * 1000 + 300;
  }

  namespace cfg {
    const unsigned int cfg = net * 1000 + 500;
  }
}

#endif // __KIEVIEW2_H__