/**
  *  kIEview2 stdafx
  *
  *  Include file for standard system include files,
  *  or project specific include files that are used frequently,
  *  but are changed infrequently.
  *
  *  @filesource
  *  @copyright    Copyright (c) 2004-2005 Kuba 'nix' Niegowski
  *  @link         svn://konnekt.info/kieview2/ kIEview2 plugin SVN Repo
  *  @version      $Revision$
  *  @modifiedby   $LastChangedBy$
  *  @lastmodified $Date$
  */

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER        // Allow use of features specific to Windows XP or later.
#define WINVER 0x0501    // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT    // Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501  // Change this to the appropriate value to target other versions of Windows.
#endif            

#ifndef _WIN32_WINDOWS    // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE      // Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600  // Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN    // Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <richedit.h>
#include <OleIdl.h>
#include <richole.h>
#include <shellapi.h>
#pragma comment(lib, "shell32.lib")

#include <map>
#include <list>
#include <deque>
#include <stdstring.h>
#include <string>
#include <fstream>

#ifdef _DEBUG
  #pragma comment(lib, "stamina_d.lib")
#else
  #pragma comment(lib, "stamina.lib")
#endif

#include <boost/signal.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <stamina/helpers.h>
#include <stamina/object.h>
#include <stamina/objectimpl.h>
#include <stamina/exception.h>
#include <stamina/criticalsection.h>
#include <stamina/string.h>
#include <stamina/time64.h>
#include <stamina/regex.h>
#include <stamina/timer.h>
#include <stamina/DataTable/Crypt.h>

using namespace Stamina;
using namespace std;

#include <konnekt/plug_export.h>
#include <konnekt/ui.h>
#include <konnekt/plug_func.h>
#include <konnekt/knotify.h>
#include <konnekt/ksound.h>
#include <konnekt/sms.h>
#include <konnekt/tabletka.h>
#include <konnekt/ui_message_controls.h>
#include <konnekt/lib.h>
#include <konnekt/plugsNET.h>

using namespace Konnekt;
using namespace boost;

#pragma comment(lib, "glib-2.0.lib")
#ifdef _DEBUG
  #pragma comment(lib, "glibmm-2.4d.lib")
  #pragma comment(lib, "xml++-2.6d.lib")
#else
  #pragma comment(lib, "glibmm-2.4.lib")
  #pragma comment(lib, "xml++-2.6.lib")
#endif

#include <libxml++/libxml++.h>

#include <XZip.h>
#include <XUnzip.h>

#include "../Project/resource.h"