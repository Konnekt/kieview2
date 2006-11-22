/**
  *  kIEview2 header file
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

    const unsigned int logo = ico + 1;
    const unsigned int link = ico + 2;
    const unsigned int copy = ico + 3;
    const unsigned int save = ico + 4;
    const unsigned int emots = ico + 5;
    const unsigned int bold = ico + 6;
    const unsigned int italic = ico + 7;
    const unsigned int underline = ico + 8;
    const unsigned int color = ico + 9;
  }

  namespace act {
    const unsigned int act = net * 1000 + 200;

    namespace popup {
      const unsigned int popup = act + 100;

      const unsigned int openUrl = popup + 1;
      const unsigned int copyUrl = popup + 2;
      const unsigned int saveImage = popup + 3;
      const unsigned int copySelection = popup + 4;
      const unsigned int selectAll = popup + 5;
      const unsigned int history = popup + 6;
      const unsigned int clear = popup + 7;
    }

    namespace formatTb {
      const unsigned int formatTb = act + 120;

      const unsigned int bold = formatTb + 1;
      const unsigned int italic = formatTb + 2;
      const unsigned int underline = formatTb + 3;
      const unsigned int emots = formatTb + 4;
      const unsigned int color = formatTb + 5;
    }
  }

  namespace cfg {
    const unsigned int cfg = net * 1000 + 500;

    const unsigned int useEmots = cfg + 1;
    const unsigned int emotsDir = cfg + 2;
    const unsigned int showFormatTb = cfg + 3;
  }
}

#endif // __KIEVIEW2_H__