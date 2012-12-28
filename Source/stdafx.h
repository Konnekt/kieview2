/**
  *  kIEview2 stdafx
  *
  *  Include file for standard system include files,
  *  or project specific include files that are used frequently,
  *  but are changed infrequently.
  *
  *  @filesource
  *  @copyright    Copyright (c) 2005-2006 Sijawusz Pur Rahnama
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

#include <hash_map>
#include <list>
#include <deque>
#include <stack>
#include <stdstring.h>
#include <string>

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "ComCtl32.lib")
#pragma comment(lib, "Msimg32.lib")

#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "DevILU.lib")

#pragma comment(lib, "glib-2.0.lib")
#pragma comment(lib, "Swift.lib")
#ifdef _DEBUG
  #pragma comment(lib, "stamina_d.lib")
  #pragma comment(lib, "stamina_ui_d.lib")
  #pragma comment(lib, "listwnd_d.lib")
  #pragma comment(lib, "datatable_d.lib")
  #pragma comment(lib, "glibmm-2.4d.lib")
  #pragma comment(lib, "xml++-2.6d.lib")
#else
  #pragma comment(lib, "stamina.lib")
  #pragma comment(lib, "stamina_ui.lib")
  #pragma comment(lib, "listwnd.lib")
  #pragma comment(lib, "datatable.lib")
  #pragma comment(lib, "glibmm-2.4.lib")
  #pragma comment(lib, "xml++-2.6.lib")
#endif

#include <boost/signal.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <Stamina/Helpers.h>
#include <Stamina/Object.h>
#include <Stamina/ObjectImpl.h>
#include <Stamina/Exception.h>
#include <Stamina/CriticalSection.h>
#include <Stamina/String.h>
#include <Stamina/Time64.h>
#include <Stamina/Regex.h>
#include <Stamina/Timer.h>
#include <Stamina/FindFile.h>
#include <Stamina/FindFileFiltered.h>
#include <Stamina/SimXML.h>
#include <Stamina/WinHelper.h>

#include <Stamina/DataTable/DataTable.h>
#include <Stamina/DataTable/FileBin.h>
#include <Stamina/DataTable/Crypt.h>

#include <Stamina/ListWnd/ListWnd.h>
#include <Stamina/ListWnd/ListView.h>
#include <Stamina/UI/DrawableButton.h>
#include <Stamina/UI/ToolTipX.h>
#include <Stamina/UI/ImageIL.h>

using namespace Stamina;
using namespace std;
using namespace boost;

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

#include <Swift/Swift.h>

using namespace Konnekt;
using namespace Swift;

#include <libxml++/libxml++.h>

#include "../Project/resource.h"
