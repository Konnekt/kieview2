/**
  *  IECtrl class
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
#include "IECtrl.h"

#define DISPID_BEFORENAVIGATE2 250

IECtrl::External* IECtrl::m_pExternal = NULL;
IECtrl* IECtrl::m_pList = NULL;
CRITICAL_SECTION IECtrl::m_mutex;

bool IECtrl::m_bInited = false;
bool IECtrl::m_bOnCopyEmptySel = true;
bool IECtrl::m_bAutoCopySel = false;

void IECtrl::init() {
  if (m_bInited) return;

  m_bInited = true;
  InitializeCriticalSection(&m_mutex);

  if (FAILED(OleInitialize(NULL))) {
    MessageBox(NULL, "OleInitialize failed", "IECtrl", MB_ICONERROR);
  }
}

void IECtrl::deinit() {
  while (m_pList != NULL) {
    delete m_pList;
  }
  delete m_pExternal;
  DeleteCriticalSection(&m_mutex);
  m_bInited = false;
}

IECtrl::IECtrl(HWND parent, int x, int y, int cx, int cy, bool staticEdge) {
  MSG msg;
  IOleObject* pOleObject = NULL;
  IOleInPlaceObject* pOleInPlace = NULL;

  m_hParentWnd = parent;
  m_pPrev = m_pNext = NULL;
  m_hWnd = NULL;
  m_pEventSink = NULL;
  m_pClientSite = NULL;
  m_bClosed = false;
  m_bSandbox = true;
  m_bGetSelection = false;
  m_pDropTarget = new DropTarget(this);

  m_pAnchorClickListener = NULL;
  m_pPopupMenuListener = NULL;
  m_pDropListener = NULL;
  m_pKeyDownListener = NULL;
  m_pScriptMessageListener = NULL;

  m_pWebBrowser = NULL;
  m_pConnectionPoint = NULL;
  m_szSelectedText = NULL;

  if (SUCCEEDED(CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC, IID_IWebBrowser2, (LPVOID*)&m_pWebBrowser))) {
    // m_pWebBrowser->put_RegisterAsBrowser(VARIANT_FALSE);
    // m_pWebBrowser->put_RegisterAsDropTarget(VARIANT_FALSE);
    m_pWebBrowser->put_Offline(m_bSandbox ? VARIANT_TRUE : VARIANT_FALSE);

    if (SUCCEEDED(m_pWebBrowser->QueryInterface(IID_IOleObject, (void**)&pOleObject))) {
      m_rcClient.left = x;
      m_rcClient.top = y;
      m_rcClient.right = x + cx;
      m_rcClient.bottom = y + cy;
      m_pClientSite = new ClientSite(this);
      pOleObject->SetClientSite(m_pClientSite);
      pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, &msg, m_pClientSite, 0, m_hParentWnd, &m_rcClient);
      pOleObject->Release();
    } else {
      MessageBox(NULL, "IOleObject failed", "IECtrl", MB_ICONERROR);
    }

    if (SUCCEEDED(m_pWebBrowser->QueryInterface(IID_IOleInPlaceObject, (void**)&pOleInPlace))) {
      pOleInPlace->GetWindow(&m_hWnd);
      pOleInPlace->Release();
    } else {
      MessageBox(NULL, "IOleInPlaceObject failed", "IECtrl", MB_ICONERROR);
    }

    if (staticEdge) {
      LONG style = GetWindowLong(m_hWnd, GWL_EXSTYLE);
      style |= (WS_EX_STATICEDGE);
      SetWindowLong(m_hWnd, GWL_EXSTYLE, style);
    }

    IConnectionPointContainer* pCPContainer;
    if (SUCCEEDED(m_pWebBrowser->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPContainer))) {
      if (SUCCEEDED(pCPContainer->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_pConnectionPoint))) {
        m_pEventSink = new EventSink(this);
        if (FAILED(m_pConnectionPoint->Advise((IUnknown *)m_pEventSink, &m_dwCookie))) {
          MessageBox(NULL, "Advise failed", "IECtrl", MB_ICONERROR);
        }
      }
      pCPContainer->Release();
    }
    setUserWndProc((WNDPROC)SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG) IECtrl::IECtrlWindowProcedure));
  }

  EnterCriticalSection(&m_mutex);
  m_pNext = m_pList;
  if (m_pNext != NULL) {
    m_pNext->m_pPrev = this;
  }
  m_pList = this;
  LeaveCriticalSection(&m_mutex);

  clear();
}

IECtrl::~IECtrl() {
  IOleObject* pOleObject = NULL;

  if (SUCCEEDED(m_pWebBrowser->QueryInterface(IID_IOleObject, (void**)&pOleObject)) && pOleObject != NULL) {
    pOleObject->SetClientSite(NULL);
    pOleObject->Release();
  } else {
    MessageBox(NULL, "IOleObject failed", "IECtrl", MB_ICONERROR);
  }

  EnterCriticalSection(&m_mutex);
  if (m_pList == this) {
    m_pList = m_pNext;
  } else if (m_pPrev != NULL) {
    m_pPrev->m_pNext = m_pNext;
  }
  if (m_pNext != NULL) {
    m_pNext->m_pPrev = m_pPrev;
  }
  LeaveCriticalSection(&m_mutex);

  if (m_pConnectionPoint != NULL) {
    m_pConnectionPoint->Unadvise(m_dwCookie);
    m_pConnectionPoint->Release();
  }
  if (m_pEventSink != NULL) {
    delete m_pEventSink;
  }
  if (m_szSelectedText != NULL) {
    delete m_szSelectedText;
  }
  m_pWebBrowser->Release();
  DestroyWindow(m_hWnd);
  if (m_pClientSite != NULL) {
    delete m_pClientSite;
  }
  if (m_pDropTarget != NULL) {
    delete m_pDropTarget;
  }
}

void IECtrl::setUserWndProc(WNDPROC wndProc) {
  m_fUserWndProc = wndProc;
}

WNDPROC IECtrl::getUserWndProc() {
  return m_fUserWndProc;
}

LRESULT CALLBACK IECtrl::IECtrlServerWindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  IECtrl *ctrl = IECtrl::get(GetParent(GetParent(hwnd)));
  if (ctrl != NULL) {
    switch (message) {
      case WM_KEYUP:
        if (LOWORD(wParam) == VK_ESCAPE && !(GetKeyState(VK_SHIFT) & 0x8000) && !(GetKeyState(VK_CONTROL) & 0x8000) && !(GetKeyState(VK_MENU) & 0x8000)) {
          SendMessage(GetParent(GetParent(GetParent(hwnd))), WM_COMMAND, IDCANCEL, 0);
        }
        break;
      case WM_KEYDOWN:
        ctrl->translateAccelerator(message, wParam, lParam);
        break;
      case WM_SYSKEYDOWN:
        ctrl->translateAccelerator(message, wParam, lParam);
        break;
      case WM_SETFOCUS:
        if (ctrl->setFocus((HWND)wParam)) {
          return true;
        }
        break;
      case WM_LBUTTONDOWN:
        POINT pt;
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);

        if (getAutoCopySel()) {
          ctrl->m_bGetSelection = true;
        }
        if (ctrl->mouseClick(pt)) {
          return true;
        }
        break;
      case WM_LBUTTONUP:
        if (getAutoCopySel()) {
          ctrl->copySelection();
        }
        break;
      case WM_LBUTTONDBLCLK:
        HRESULT hr = CallWindowProc(ctrl->getUserWndProc(), hwnd, message, wParam, lParam);
        if (getAutoCopySel()) {
          ctrl->m_bGetSelection = true;
          ctrl->copySelection();
        }
        return hr;
    }
    return CallWindowProc(ctrl->getUserWndProc(), hwnd, message, wParam, lParam);
  }
  return DefWindowProc (hwnd, message, wParam, lParam);
}

LRESULT CALLBACK IECtrl::IECtrlDocWindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  IECtrl *ctrl = IECtrl::get(GetParent(hwnd));

  if (ctrl != NULL) {
    WNDPROC oldWndProc = ctrl->getUserWndProc();
    if (message == WM_PARENTNOTIFY && wParam == WM_CREATE) {
      SetWindowLong(hwnd, GWL_WNDPROC, (LONG) oldWndProc);
      ctrl->setUserWndProc((WNDPROC) SetWindowLong((HWND)lParam, GWL_WNDPROC, (LONG) IECtrl::IECtrlServerWindowProcedure));
    }
    return CallWindowProc(oldWndProc, hwnd, message, wParam, lParam);
  }
  return DefWindowProc (hwnd, message, wParam, lParam);
}

LRESULT CALLBACK IECtrl::IECtrlWindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  IECtrl *ctrl = IECtrl::get(hwnd);

  if (ctrl != NULL) {
    WNDPROC oldWndProc = ctrl->getUserWndProc();
    if (message == WM_PARENTNOTIFY && wParam == WM_CREATE) {
      SetWindowLong(hwnd, GWL_WNDPROC, (LONG) oldWndProc);
      ctrl->setUserWndProc((WNDPROC) SetWindowLong((HWND)lParam, GWL_WNDPROC, (LONG) IECtrl::IECtrlDocWindowProcedure));
    }
    return CallWindowProc(oldWndProc, hwnd, message, wParam, lParam);
  }
  return DefWindowProc (hwnd, message, wParam, lParam);
}

IHTMLDocument2* IECtrl::getDocument() {
  HRESULT hr = S_OK;
  IHTMLDocument2 *document = NULL;
  IDispatch *dispatch = NULL;

  if (SUCCEEDED(m_pWebBrowser->get_Document(&dispatch)) && dispatch != NULL)  {
    hr = dispatch->QueryInterface(IID_IHTMLDocument2, (void **)&document);
    dispatch->Release();
  }
  return document;
}

void IECtrl::setWindowPos(int x, int y, int cx, int cy)  {
  m_rcClient.left = x;
  m_rcClient.top = y;
  m_rcClient.right = cx;
  m_rcClient.bottom = cy; //y + cy;

  SetWindowPos(getHWND(), HWND_TOP, x, y, cx, cy, 0);
}

void IECtrl::scrollToTop() {
  IHTMLDocument2 *document = getDocument();
  if (document != NULL) {
    IHTMLWindow2* pWindow = NULL;
    if (SUCCEEDED(document->get_parentWindow(&pWindow)) && pWindow != NULL) {
      pWindow->scrollBy(-0x01FFFFFF, -0x01FFFFFF);
      pWindow->Release();
    }
    document->Release();
  }
}

bool IECtrl::isScrollOnBottom() {
  bool isScroll = false;

  IHTMLDocument2 *document = getDocument();
  if (document != NULL) {
    IHTMLDocument2 *pDocument = NULL;
    if (SUCCEEDED(document->QueryInterface( IID_IHTMLDocument2, (void**)&pDocument)) && pDocument != NULL) {
      IHTMLElement *pBody = NULL;
      if (SUCCEEDED(pDocument->get_body(&pBody)) && pBody != NULL) {
        IHTMLElement2 *pElement = NULL;
        if (SUCCEEDED(pBody->QueryInterface(IID_IHTMLElement2,(void**)&pElement)) && pElement != NULL) {
            long sh, ch, st;
            pElement->get_scrollHeight(&sh);
            pElement->get_clientHeight(&ch);
            pElement->get_scrollTop(&st);

            isScroll = (sh <= ch + st);
            pElement->Release();
        }
        pBody->Release();
      }
      pDocument->Release();
    }
    document->Release();
  }
  return isScroll;
}

void IECtrl::scrollToBottom() {
  IHTMLDocument2 *document = getDocument();
  if (document != NULL) {
    IHTMLElementCollection *collection;
    IHTMLElement *element;
    IDispatch *dispatch;

    if (SUCCEEDED(document->get_all(&collection)) && collection != NULL) {
      long len;

      if (SUCCEEDED(collection->get_length(&len))) {
        VARIANT  variant;
        variant.vt = VT_I4;
        variant.lVal = len-1;

        if (SUCCEEDED(collection->item(variant, variant, &dispatch)) && dispatch != NULL) {
          if (SUCCEEDED(dispatch->QueryInterface(IID_IHTMLElement,(void**)&element)) && element != NULL) {
            variant.vt = VT_BOOL;
            variant.boolVal = VARIANT_FALSE;
            if (SUCCEEDED(element->scrollIntoView(variant))) {
              //
            }
            element->Release();
          }
          dispatch->Release();
        }
      }
      collection->Release();
    }

    IHTMLWindow2* pWindow = NULL;
    if (SUCCEEDED(document->get_parentWindow(&pWindow)) && pWindow != NULL) {
      pWindow->scrollBy(-0x01FFFFFF, 0x01FFFFFF);
      pWindow->Release();
    }
    document->Release();
  }
}

void IECtrl::write(const WCHAR *text) {
  if (!m_bClosed) {
    IHTMLDocument2 *document = getDocument();
    if (document != NULL)  {
      SAFEARRAY *safe_array = SafeArrayCreateVector(VT_VARIANT, 0, 1);
      if (safe_array != NULL) {
        VARIANT *variant;
        BSTR bstr;
        SafeArrayAccessData(safe_array, (LPVOID *)&variant);
        variant->vt = VT_BSTR;
        variant->bstrVal = bstr = SysAllocString(text);
        SafeArrayUnaccessData(safe_array);
        document->write(safe_array);
        // SysFreeString(bstr); // -> SafeArrayDestroy should be enough
        SafeArrayDestroy(safe_array);
      }
      document->Release();
    }
  } else {
    MessageBox(NULL, "Can't write to closed document", "IECtrl", MB_ICONERROR);
  }
  m_bGetFocus = false;
}

void IECtrl::write(const char *text) {
  wchar_t *wcsTemp = _com_util::ConvertStringToBSTR(text);
  write(wcsTemp);
  SysFreeString(wcsTemp);
}

void IECtrl::close() {
  IHTMLDocument2 *document = getDocument();
  if (document != NULL) {
    if (!m_bClosed) {
      document->close();
    }
    m_bClosed = true;
    document->Release();
  }
  m_bGetFocus = false;
}

void IECtrl::navigate(const char *url) {
  int textLen = strlen(url) + 1;
  WCHAR *tTemp = new WCHAR[textLen];

  MultiByteToWideChar(CP_ACP, 0, url, -1, tTemp, textLen);
  m_pWebBrowser->Navigate(tTemp, NULL, NULL, NULL, NULL);
  delete tTemp;
}

void IECtrl::clear() {
  IHTMLDocument2 *document = getDocument();
  if (document == NULL) {
    m_pWebBrowser->Navigate(L"about:blank", NULL, NULL, NULL, NULL);
    HRESULT hr = S_OK;
    IHTMLDocument2 *document = NULL;
    while (document == NULL && hr == S_OK) {
      Sleep(0);
      IDispatch *dispatch = NULL;
      if (SUCCEEDED(m_pWebBrowser->get_Document(&dispatch)) && dispatch != NULL) {
        hr = dispatch->QueryInterface(IID_IHTMLDocument2, (void **)&document);
        dispatch->Release();
      }
    }
    if (document != NULL) 
      document->Release();
  } else {
    if (!m_bClosed) {
      document->close();
    }
    VARIANT open_name;
    VARIANT open_features;
    VARIANT open_replace;
    IDispatch * open_window  = NULL;
    VariantInit(&open_name);
    open_name.vt = VT_BSTR;
    open_name.bstrVal = SysAllocString(L"_self");
    VariantInit(&open_features);
    VariantInit(&open_replace);

    HRESULT hr = document->open(SysAllocString(L"text/html"), open_name, open_features, open_replace, &open_window);
    if (hr == S_OK) {
      m_pWebBrowser->Refresh();
    }
    if (open_window != NULL) {
      open_window->Release();
    }
    document->Release();
  }
  m_bGetFocus = false;
  m_bClosed = false;
}

void IECtrl::enableSandbox(bool bSandbox) {
  m_bSandbox = bSandbox;
  if (m_pWebBrowser) {
    m_pWebBrowser->put_Offline(m_bSandbox ? VARIANT_TRUE : VARIANT_FALSE);
  }
}

std::string IECtrl::humanize(const char* text) {
  RegEx reg;
  reg.setSubject(text);

  // usuwamy znaki nowej linii
  reg.replaceItself("/[\r\n]/m", "");

  // wstawiamy w odpowiednich miejscach znaki nowej linii
  reg.replaceItself("#<(p|blockquote|li|[uo]l|div)[^>]*>#i", "\n");
  reg.replaceItself("#</(p|blockquote|li|[uo]l|div)>#i", "\n");
  reg.replaceItself("#<br ?/?>#i", "\n");

  // usuwamy wielokrotne znaki nowej linii
  reg.replaceItself("/[\n]{3,}/", "\n\n\n");

  // formatujemy listy
  reg.replaceItself("/<li>/i", "* ");

  // usuwamy znaczniki html
  reg.replaceItself("/<[^!>][^>]*>/m", "");
  reg.replaceItself("/<\\/[^>]*>/m", "");

  // zamieniamy znaki kodowane przez & na prawidlowe
  reg.replaceItself("/&#([\\d]{1,3});/", "\\x\\1");
  reg.replaceItself("/&lt;/", "<");
  reg.replaceItself("/&gt;/", ">");
  reg.replaceItself("/&quot;/", "\"");
  reg.replaceItself("/&nbsp;/", " ");
  reg.replaceItself("/&amp;/", "&");

  // dodajemy windowsowe powroty karetki
  reg.replaceItself("/\n/", "\r\n");

  return reg.getSubject();
}

bool IECtrl::copySelection(bool gettext) {
  bool result = false;
  if (m_bGetSelection) {
    char* text = getSelection(gettext);
    if (text) {
      HGLOBAL hMem = GlobalAlloc(GHND | GMEM_DDESHARE, strlen(text) + 1);
      if (hMem) {
        char* buf = (char*) GlobalLock(hMem);
        strncpy(buf, text, strlen(text));
        buf[strlen(text)] = '\0';
        GlobalUnlock(hMem);

        if (OpenClipboard(getHWND())) {
          EmptyClipboard();
          SetClipboardData(CF_TEXT, hMem);
          CloseClipboard();

          result = true;
        }
      }
    }
  }
  m_bGetSelection = false;
  return result;
}

char * IECtrl::getSelection(bool gettext) {
  if (m_szSelectedText != NULL) {
    delete m_szSelectedText;
    m_szSelectedText = NULL;
  }
  BSTR selectedTextW = getSelectionFunc(gettext);
  if (selectedTextW == NULL || wcslen(selectedTextW) == 0) {
    return NULL;
  }
  int len = wcslen(selectedTextW)+1;
  char *str = new char[len];

  WideCharToMultiByte(CP_ACP, 0, selectedTextW, len, str, len, NULL, FALSE);
  delete selectedTextW;

  if (!gettext) {
    string temp = humanize(str);
    ZeroMemory(str, len);
    strncpy(str, (char*) temp.c_str(), len - 1);
  }
  return m_szSelectedText = str;
}

BSTR IECtrl::getSelectionFunc(bool gettext) {
  BSTR text = NULL;
  IHTMLDocument2 *document = getDocument();

  if (document != NULL) {
    IHTMLSelectionObject *pSelection = NULL;
    if (SUCCEEDED(document->get_selection(&pSelection)) && pSelection != NULL) {
      IDispatch *pDisp = NULL;
      if (SUCCEEDED(pSelection->createRange(&pDisp)) &&  pDisp != NULL) {
        IHTMLTxtRange *pRange = NULL;
        if (SUCCEEDED(pDisp->QueryInterface(IID_IHTMLTxtRange, (void**)&pRange))) {
          if (gettext) {
            if (SUCCEEDED(pRange->get_text(&text))) {
              if (text != NULL) {
                wchar_t *newtext = new wchar_t[wcslen(text)+1];
                wcscpy(newtext, text);
                text = newtext;
              }
            }
          } else {
            if (SUCCEEDED(pRange->get_htmlText(&text))) {
              if (text != NULL) {
                wchar_t *newtext = new wchar_t[wcslen(text)+1];
                wcscpy(newtext, text);
                text = newtext;
              }
            }
          }
          pRange->Release();
        }
        pDisp->Release();
      }
      if (text != NULL && getOnCopyEmptySel()) {
        pSelection->empty();
      }
      pSelection->Release();
    }
    document->Release();
  }
  return text;
}

IECtrl* IECtrl::get(HWND hwnd) {
  IECtrl *ptr;
  if (m_pList == NULL) {
    return NULL;
  }
  EnterCriticalSection(&m_mutex);
  for (ptr = m_pList; ptr != NULL; ptr = ptr->m_pNext) {
    if (ptr->m_hWnd == hwnd) break;
  }
  LeaveCriticalSection(&m_mutex);
  return ptr;
}

IECtrl::External* IECtrl::getExternal() {
  if (!m_pExternal) {
    m_pExternal = new External;
  }
  return m_pExternal;
}

HWND IECtrl::getHWND() {
  return m_hWnd;
}

void IECtrl::translateAccelerator(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  char chr = (char)LOWORD(MapVirtualKey(wParam, 2));
  if (m_pKeyDownListener != NULL && m_pKeyDownListener->keyDown(wParam, lParam)
    || m_pKeyDownListener == NULL && GetKeyState(VK_CONTROL) < 0 && (chr == 'C' || chr == 'c' || chr == 'A' || chr == 'a'))
  {
    IOleInPlaceActiveObject* pIOIPAO;
    if (SUCCEEDED(m_pWebBrowser->QueryInterface(IID_IOleInPlaceActiveObject, (void**)&pIOIPAO))) {
      MSG msg;
      msg.message = uMsg;
      msg.wParam = wParam;
      msg.lParam = lParam;
      pIOIPAO->TranslateAccelerator(&msg);
      pIOIPAO->Release();
    }
  }
}

BSTR IECtrl::getHrefFromAnchor(IHTMLElement *element) {
  if (!element) return NULL;

  IHTMLAnchorElement* pAnchor;
  if (SUCCEEDED(element->QueryInterface(IID_IHTMLAnchorElement, (void**)&pAnchor)) && (pAnchor!=NULL)) {
    VARIANT  variant;
    BSTR url;
    if (SUCCEEDED(element->getAttribute(L"href", 2, &variant) && variant.vt == VT_BSTR)) {
      wchar_t *newtext = new wchar_t[wcslen(variant.bstrVal)+1];
      wcscpy(newtext, variant.bstrVal);
      url = newtext;
      SysFreeString(variant.bstrVal);
    }
    pAnchor->Release();
    return url;
  } else {
    IHTMLElement * parent;
    if (SUCCEEDED(element->get_parentElement(&parent)) && (parent != NULL)) {
      BSTR url = getHrefFromAnchor(parent);
      parent->Release();
      return url;
    }
  }
  return NULL;
}

bool IECtrl::mouseClick(POINT pt) {
  bool result = false;
  if (GetFocus() != m_hWnd) {
    m_bGetFocus = true;
  }

  IHTMLDocument2 *document = getDocument();
  if (document != NULL) {
    IHTMLElement *element;
    if (SUCCEEDED(document->elementFromPoint(pt.x, pt.y, &element)) && element != NULL) {
      BSTR url = getHrefFromAnchor(element);
      if (url != NULL) {
        /*
        if ((GetKeyState(VK_SHIFT) & 0x8000) && !(GetKeyState(VK_CONTROL) & 0x8000) && !(GetKeyState(VK_MENU) & 0x8000)) 
          SendMessage(GetParent(m_hWnd), WM_COMMAND, IDCANCEL, 0);
        */
        int i = wcslen(url);
        char *tTemp = new char[i + 1];
        WideCharToMultiByte(CP_ACP, 0, url, -1, tTemp, i + 1, NULL, NULL);
        if (m_pAnchorClickListener != NULL) {
          m_pAnchorClickListener->anchorClicked(tTemp, this);
        }
        delete tTemp;
        delete url;
        result = true;
      }
      element->Release();
    }
    document->Release();
  }
  return result;
}

bool IECtrl::setFocus(HWND prevFocus) {
  if (GetFocus() != m_hWnd && !m_bGetFocus) {
    SetFocus(prevFocus);
    return true;
  }
  return m_bGetFocus = false;
}

void IECtrl::saveDocument() {
  IHTMLDocument2 *document = getDocument();
  if (document != NULL) {
    BSTR bCmd = SysAllocString(L"SaveAs");
    VARIANT_BOOL vb;
    VARIANT vValue;
    vValue.vt = VT_BOOL;
    vValue.boolVal = TRUE;
    document->execCommand(bCmd, VARIANT_FALSE, vValue, &vb);
    SysFreeString(bCmd);
    document->Release();
  }
}

bool IECtrl::callJScript(const char* szFunc, Var &args, Var *ret) {
  IHTMLDocument2 *document = getDocument();
  bool bRet = false;

  if (document != NULL) {
    IDispatch * dispatch;
    if (!document->get_Script(&dispatch) && dispatch != NULL) {
      DISPID dispid = NULL;
      LPOLESTR str = _com_util::ConvertStringToBSTR(szFunc);

      if (!dispatch->GetIDsOfNames(IID_NULL, &str, 1, LOCALE_SYSTEM_DEFAULT, &dispid)) {
        DISPPARAMS dispparams;
        dispparams.cArgs = args.length();
        dispparams.rgvarg = new VARIANT[dispparams.cArgs];
        dispparams.cNamedArgs = 0;
        dispparams.rgdispidNamedArgs = NULL;

        for (UINT i = 0; i < dispparams.cArgs; i++) {
          args[i].getVariant(&dispparams.rgvarg[dispparams.cArgs-i-1]);
        }

        EXCEPINFO excepInfo; memset(&excepInfo, 0, sizeof(excepInfo));
        UINT nArgErr = (UINT)-1;  // initialize to invalid arg
        VARIANT vRet;
        VariantInit(&vRet);

        if (!dispatch->Invoke(dispid, IID_NULL, 0, DISPATCH_METHOD, &dispparams, &vRet, &excepInfo, &nArgErr)) {
          bRet = true;
        }
        for (UINT i = 0; i < dispparams.cArgs; i++) {
          VariantClear(&dispparams.rgvarg[i]);
        }
        delete [] dispparams.rgvarg;

        if (ret != NULL) {
          ret->setValue(vRet);
        }
        VariantClear(&vRet);
      }
      SysFreeString(str);
      dispatch->Release();
    }
    document->Release();
  }
  return bRet;
}

void IECtrl::selectAll() {
  IHTMLDocument2 *document = getDocument();
  if (document != NULL) {
    VARIANT_BOOL b = FALSE;
    _variant_t v;
    document->execCommand(_bstr_t("SelectAll"), b, v, &b);
    document->Release();
  }
}

DWORD IECtrl::connectEvents(IHTMLElement* pElem, IDispatch* pDisp) {
  IConnectionPointContainer* pCPC = NULL;
  IConnectionPoint* pCP = NULL;
  DWORD dwCookie;

  if (SUCCEEDED(pElem->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC))) {
    if (SUCCEEDED(pCPC->FindConnectionPoint(DIID_HTMLElementEvents2, &pCP))) {
      if (FAILED(pCP->Advise((IUnknown*)pDisp, &dwCookie))) {
        MessageBox(NULL, "Events advise failed", "IECtrl", MB_ICONERROR);
      }
      pCP->Release();
    }
    pCPC->Release();
  }
  return dwCookie;
}

void IECtrl::disconnectEvents(IHTMLElement* pElem, DWORD dwCookie) {
  IConnectionPointContainer* pCPC = NULL;
  IConnectionPoint* pCP = NULL;

  if (SUCCEEDED(pElem->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC))) {
    if (SUCCEEDED(pCPC->FindConnectionPoint(DIID_HTMLElementEvents2, &pCP))) {
      if (FAILED(pCP->Unadvise(dwCookie))) {
        MessageBox(NULL, "Events unadvise failed", "IECtrl", MB_ICONERROR);
      }
      pCP->Release();
    }
    pCPC->Release();
  }
} 

/*
 * EventSink
 */

IECtrl::EventSink::EventSink(IECtrl * pCtrl) {
  m_pCtrl = pCtrl;
  m_cRef = 0;
}

IECtrl::EventSink::~EventSink() {
  _ASSERT(m_cRef == 0);
}

STDMETHODIMP IECtrl::EventSink::QueryInterface(REFIID riid, PVOID *ppv) {
  *ppv = NULL;
  if (IID_IUnknown == riid) {
    *ppv = (IUnknown*) this;
  }
  if (IID_IDispatch == riid) {
    *ppv = (IDispatch*) this;
  }
  if (DIID_DWebBrowserEvents2 == riid) {
    *ppv = (DWebBrowserEvents2*) this;
  }
  if (NULL != *ppv) {
    ((LPUNKNOWN)*ppv)->AddRef();
    return NOERROR;
  }
  return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) IECtrl::EventSink::AddRef(void) {
  ++m_cRef;
  return m_cRef;
}

STDMETHODIMP_(ULONG) IECtrl::EventSink::Release(void) {
  --m_cRef;
  return m_cRef;
}

// IDispatch
STDMETHODIMP IECtrl::EventSink::GetTypeInfoCount(UINT *ptr) { 
  return E_NOTIMPL; 
}

STDMETHODIMP IECtrl::EventSink::GetTypeInfo(UINT iTInfo, LCID lcid, LPTYPEINFO* ppTInfo) { 
  return S_OK; 
}

STDMETHODIMP IECtrl::EventSink::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) { 
  return S_OK; 
}

STDMETHODIMP IECtrl::EventSink::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid , WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO*pExcepInfo, UINT*puArgErr) 
{
  if (!pDispParams) return E_INVALIDARG;

  switch (dispIdMember) {
    case DISPID_BEFORENAVIGATE2:
      BeforeNavigate2(pDispParams->rgvarg[6].pdispVal,
                      pDispParams->rgvarg[5].pvarVal,
                      pDispParams->rgvarg[4].pvarVal,
                      pDispParams->rgvarg[3].pvarVal,
                      pDispParams->rgvarg[2].pvarVal,
                      pDispParams->rgvarg[1].pvarVal,
                      pDispParams->rgvarg[0].pboolVal);
      return S_OK;
  }
  return DISP_E_MEMBERNOTFOUND;
}

// DWebBrowserEvents2
void IECtrl::EventSink::StatusTextChange(BSTR text) {
}

void IECtrl::EventSink::ProgressChange(long progress, long progressMax) {
}

void IECtrl::EventSink::CommandStateChange(long command, VARIANT_BOOL enable) {
}

void IECtrl::EventSink::DownloadBegin() {
}

void IECtrl::EventSink::DownloadComplete() {
}

void IECtrl::EventSink::TitleChange(BSTR text) {
}

void IECtrl::EventSink::PropertyChange(BSTR text) {
}

void IECtrl::EventSink::BeforeNavigate2(IDispatch* pDisp, VARIANT* url, VARIANT* flags, VARIANT* targetFrameName, VARIANT* postData, VARIANT* headers, VARIANT_BOOL* cancel) 
{
  int i = wcslen(url->bstrVal);
  char* tTemp = new char[i+1];
  WideCharToMultiByte(CP_ACP, 0, url->bstrVal, -1, tTemp, i+1, NULL, NULL);

  if (strcmp(tTemp, "about:blank")) {
    // sytuacja bezpieczenstwa gdy mouseClick nie przechwyci odnosnika
    if (m_pCtrl->m_pAnchorClickListener != NULL) {
      m_pCtrl->m_pAnchorClickListener->anchorClicked(tTemp, m_pCtrl);
    }
    *cancel = VARIANT_TRUE;
  }
  delete tTemp;
}

void IECtrl::EventSink::NewWindow2(IDispatch** ppDisp, VARIANT_BOOL* cancel) {
}

void IECtrl::EventSink::NavigateComplete(IDispatch* pDisp, VARIANT* url) {
}

void IECtrl::EventSink::DocumentComplete(IDispatch* pDisp, VARIANT* url) {
}

void IECtrl::EventSink::OnQuit() {
}

void IECtrl::EventSink::OnVisible(VARIANT_BOOL visible) {
}

void IECtrl::EventSink::OnToolBar(VARIANT_BOOL visible) {
}

void IECtrl::EventSink::OnMenuBar(VARIANT_BOOL visible) {
}

void IECtrl::EventSink::OnStatusBar(VARIANT_BOOL visible) {
}

void IECtrl::EventSink::OnFullScreen(VARIANT_BOOL visible) {
}

void IECtrl::EventSink::OnTheaterMode(VARIANT_BOOL visible) {
}

void IECtrl::EventSink::WindowSetResizable(VARIANT_BOOL visible) {
}

void IECtrl::EventSink::WindowSetLeft(long val) {
}

void IECtrl::EventSink::WindowSetTop(long val) {
}

void IECtrl::EventSink::WindowSetWidth(long val) {
}

void IECtrl::EventSink::WindowSetHeight(long val) {
}

void IECtrl::EventSink::WindowClosing(VARIANT_BOOL isChildWindow, VARIANT_BOOL* cancel) {
}

void IECtrl::EventSink::ClientToHostWindow(long *cx, long *cy) {
}

void IECtrl::EventSink::SetSecureLockIcon(long val) {
}

void IECtrl::EventSink::FileDownload(VARIANT_BOOL* cancel) {
}

/*
 * ClientSite
 */

IECtrl::ClientSite::ClientSite(IECtrl * pCtrl) {
  m_pCtrl = pCtrl;
  m_cRef = 0;
}

IECtrl::ClientSite::~ClientSite() {
  _ASSERT(m_cRef == 0);
}

// IUnknown
STDMETHODIMP IECtrl::ClientSite::QueryInterface(REFIID riid, PVOID *ppv) {
  *ppv = NULL;
  if (IID_IUnknown == riid) {
    *ppv = this;
  }
  if (IID_IOleClientSite == riid) {
    *ppv = (IOleClientSite*) this;
  }
  if (IID_IOleWindow == riid || IID_IOleInPlaceSite == riid) {
    *ppv = (IOleInPlaceSite*) this;
  }
  if (IID_IDocHostUIHandler == riid) {
    *ppv = (IDocHostUIHandler*) this;
  }
  /* if (IID_IDispatch == riid) {
    *ppv = (IDispatch*) this;
  } */
  if (IID_IDocHostShowUI == riid) {
    *ppv = (IDocHostShowUI*) this;
  }
  if (IID_IServiceProvider == riid) {
    *ppv = (IServiceProvider*) this;
  }
  if (IID_IInternetSecurityManager == riid) {
    *ppv = (IInternetSecurityManager*) this;
  }
  if (NULL != *ppv) {
    ((LPUNKNOWN)*ppv)->AddRef();
    return NOERROR;
  }
  return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) IECtrl::ClientSite::AddRef(void) {
  ++m_cRef;
  return m_cRef;
}

STDMETHODIMP_(ULONG) IECtrl::ClientSite::Release(void) {
  --m_cRef;
  return m_cRef;
}

// IOleWindow
STDMETHODIMP IECtrl::ClientSite::GetWindow(HWND *phwnd) {
  *phwnd = m_pCtrl->m_hParentWnd;
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::ContextSensitiveHelp(BOOL fEnterMode) {
  return E_NOTIMPL;
}

// IOleInPlace
STDMETHODIMP IECtrl::ClientSite::CanInPlaceActivate(void) {
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::OnInPlaceActivate(void) {
  m_pCtrl->m_bInPlaceActive = TRUE;
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::OnUIActivate(void) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo) 
{
  lprcPosRect->left = m_pCtrl->m_rcClient.left;
  lprcPosRect->top = m_pCtrl->m_rcClient.top;
  lprcPosRect->right = m_pCtrl->m_rcClient.right;
  lprcPosRect->bottom = m_pCtrl->m_rcClient.bottom;
  lprcClipRect->left = m_pCtrl->m_rcClient.left;
  lprcClipRect->top = m_pCtrl->m_rcClient.top;
  lprcClipRect->right = m_pCtrl->m_rcClient.right;
  lprcClipRect->bottom = m_pCtrl->m_rcClient.bottom;

  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::Scroll(SIZE scrollExtant) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::OnUIDeactivate(BOOL fUndoable) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::OnInPlaceDeactivate(void) {
  m_pCtrl->m_bInPlaceActive = FALSE;
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::DiscardUndoState(void) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::DeactivateAndUndo(void) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::OnPosRectChange(LPCRECT lprcPosRect) {
  return E_NOTIMPL;
}

// IOleClientSite
STDMETHODIMP IECtrl::ClientSite::SaveObject(void) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::GetContainer(IOleContainer **ppContainer) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::ShowObject(void) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::OnShowWindow(BOOL fShow) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::RequestNewObjectLayout(void) {
  return E_NOTIMPL;
}

// IDocHostUIHandler
STDMETHODIMP IECtrl::ClientSite::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdTarget, IDispatch *pdispElement) {
  IOleCommandTarget * pOleCommandTarget;
  IOleWindow * pOleWindow;
  HWND hSPWnd;
  
  if (SUCCEEDED(pcmdTarget->QueryInterface(IID_IOleCommandTarget, (void**)&pOleCommandTarget))) {
    if (SUCCEEDED(pOleCommandTarget->QueryInterface(IID_IOleWindow, (void**)&pOleWindow))) {
      pOleWindow->GetWindow(&hSPWnd);

      if (m_pCtrl->m_pPopupMenuListener != NULL) {
        PopupMenuListener::MenuType type = PopupMenuListener::MenuType::Unknown;
        if (dwID == 5) {
          type = PopupMenuListener::MenuType::Anchor;
        } else if (dwID == 4) {
          type = PopupMenuListener::MenuType::Selection;
        } else if (dwID == 1) {
          type = PopupMenuListener::MenuType::Image;
        }

        PopupMenuListener::MakeAction action = m_pCtrl->m_pPopupMenuListener->popupMenu(type, *ppt, m_pCtrl);
        if (action == PopupMenuListener::MakeAction::OpenLink) {
          // Dalej ppt nie jest u¿ywane - nie robiê kopii przed konwersj¹. Jakby co to tu szukaæ b³êdu.
          ScreenToClient(hSPWnd, ppt);
          m_pCtrl->mouseClick(*ppt);
        } else if (action == PopupMenuListener::MakeAction::CopyLink) {
          SendMessage(hSPWnd, WM_COMMAND, (WPARAM)2262, (LPARAM) NULL);
        } else if (action == PopupMenuListener::MakeAction::CopySelection) {
          SendMessage(hSPWnd, WM_COMMAND, (WPARAM)15, (LPARAM) NULL);
          if (m_pCtrl->getOnCopyEmptySel()) {
            IHTMLDocument2 *document = m_pCtrl->getDocument();
            if (document != NULL) {
              IHTMLSelectionObject *pSelection = NULL;
              if (SUCCEEDED(document->get_selection(&pSelection)) && pSelection != NULL) {
                pSelection->empty();
                pSelection->Release();
              }
              document->Release();
            }
          }
        } else if (action == PopupMenuListener::MakeAction::Print) {
          // SendMessage(hSPWnd, WM_COMMAND, (WPARAM)27, (LPARAM) NULL); // print...
          SendMessage(hSPWnd, WM_COMMAND, (WPARAM)2003, (LPARAM) NULL); // print preview...
        } else if (action == PopupMenuListener::MakeAction::SaveImage) {
          SendMessage(hSPWnd, WM_COMMAND, (WPARAM)2270, (LPARAM) NULL);
        } else if (action == PopupMenuListener::MakeAction::SelectAll) {
          SendMessage(hSPWnd, WM_COMMAND, (WPARAM)31, (LPARAM) NULL);
        } else if (action == PopupMenuListener::MakeAction::ShowSource) {
          SendMessage(hSPWnd, WM_COMMAND, (WPARAM)2139, (LPARAM) NULL);
        }
      }

      /*
      if (dwID == 5) { // anchor 
        IHTMLElement *element;
        if (SUCCEEDED(pdispElement->QueryInterface(IID_IHTMLElement,(void**)&element)) && element != NULL) {
          BSTR url = m_pCtrl->getHrefFromAnchor(element);
          if (url != NULL) {
            int i = wcslen(url);
            char *tTemp = new char[i+1];
            WideCharToMultiByte(CP_ACP, 0, url, -1, tTemp, i+1, NULL, NULL);
            MessageBox(NULL, tTemp, "Anchor PopupMenu ", MB_OK); 
            delete tTemp;
            delete url;
          }
          element->Release();
        }
      }
      */
      pOleWindow->Release();
    }
    pOleCommandTarget->Release();
  }
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::GetHostInfo(DOCHOSTUIINFO *pInfo) {
  pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER; // | DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE;
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc) 
{
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::HideUI(void) {
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::UpdateUI(void) {
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::EnableModeless(BOOL fEnable) { 
  return E_NOTIMPL; 
}

STDMETHODIMP IECtrl::ClientSite::OnDocWindowActivate(BOOL fEnable) { 
  return E_NOTIMPL; 
}

STDMETHODIMP IECtrl::ClientSite::OnFrameWindowActivate(BOOL fEnable) { 
  return E_NOTIMPL; 
}

STDMETHODIMP IECtrl::ClientSite::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow) {
  return E_NOTIMPL;
}

STDMETHODIMP IECtrl::ClientSite::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID) { 
  return S_FALSE;
}

STDMETHODIMP IECtrl::ClientSite::GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw) { 
  return E_NOTIMPL; 
}

STDMETHODIMP IECtrl::ClientSite::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget) { 
  *ppDropTarget = m_pCtrl->m_pDropTarget;
  m_pCtrl->m_pDropTarget->AddRef();
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::GetExternal(IDispatch **ppDispatch) {
  *ppDispatch = m_pCtrl->getExternal();
  m_pCtrl->getExternal()->AddRef();
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut) { 
  return E_NOTIMPL; 
}

STDMETHODIMP IECtrl::ClientSite::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet) { 
  return E_NOTIMPL; 
}

// IDocHostShowUI

HRESULT IECtrl::ClientSite::ShowHelp(HWND hwnd, LPOLESTR pszHelpFile, UINT nCommand, DWORD dwData, POINT ptMouse, IDispatch * pDispatchObjectHit) {
  return S_OK;
}

HRESULT IECtrl::ClientSite::ShowMessage(HWND hWnd, LPOLESTR lpstrText, LPOLESTR lpstrCaption, DWORD dwType, LPOLESTR lpstrHelpFile, DWORD dwHelpContext, LRESULT * plResult)
{
  const char * text = _com_util::ConvertBSTRToString(lpstrText);
  if (m_pCtrl->m_pScriptMessageListener != NULL) {
    *plResult = m_pCtrl->m_pScriptMessageListener->showMessage(hWnd, text, dwType);
  } else {
    *plResult = MessageBox(hWnd, text, "JScript Message", dwType);
  }
  delete [] text;
  return S_OK;
}

// IServiceProvider
STDMETHODIMP IECtrl::ClientSite::QueryService(REFGUID guidService, REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) {
  if (guidService == SID_SInternetSecurityManager && m_pCtrl->m_bSandbox) {
    if (riid == IID_IInternetSecurityManager) {
      return this->QueryInterface(riid, ppvObject);
    } else {
      return E_NOINTERFACE;
    }
  }
  return E_UNEXPECTED;
}

// IInternetSecurityManager
STDMETHODIMP IECtrl::ClientSite::SetSecuritySite(IInternetSecurityMgrSite *pSite) {
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP IECtrl::ClientSite::GetSecuritySite(IInternetSecurityMgrSite **ppSite) {
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP IECtrl::ClientSite::MapUrlToZone(LPCWSTR pwszUrl, DWORD *pdwZone, DWORD dwFlags) {
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP IECtrl::ClientSite::GetSecurityId(LPCWSTR pwszUrl, BYTE *pbSecurityId, DWORD *pcbSecurityId, DWORD_PTR dwReserved) {
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP IECtrl::ClientSite::ProcessUrlAction(LPCWSTR pwszUrl, DWORD dwAction, BYTE *pPolicy, DWORD cbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved)
{
  if (cbPolicy != 4) return INET_E_DEFAULT_ACTION;
  PBOOL pBool = (PBOOL)pPolicy;

  if (dwAction == URLACTION_SCRIPT_RUN) {
    *pBool = URLPOLICY_DISALLOW;
  } else if (dwAction == URLACTION_JAVA_PERMISSIONS) {
    *pBool = URLPOLICY_JAVA_PROHIBIT;
  } else {
    *pBool = URLPOLICY_DISALLOW;
  }
  return S_OK;
}

STDMETHODIMP IECtrl::ClientSite::QueryCustomPolicy(LPCWSTR pwszUrl, REFGUID guidKey, BYTE **ppPolicy, DWORD *pcbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwReserved)
{
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP IECtrl::ClientSite::SetZoneMapping(DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags) {
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP IECtrl::ClientSite::GetZoneMappings(DWORD dwZone, IEnumString **ppenumString, DWORD dwFlags) {
  return INET_E_DEFAULT_ACTION;
}

/*
 * DropTarget
 */

IECtrl::DropTarget::DropTarget(IECtrl * pCtrl) {
  m_pCtrl = pCtrl;
  m_cRef = 0;
}

IECtrl::DropTarget::~DropTarget() {
  _ASSERT(m_cRef == 0);
}

// IUnknown
STDMETHODIMP IECtrl::DropTarget::QueryInterface(REFIID riid, PVOID *ppv) {
  *ppv = NULL;
  if (IID_IUnknown == riid) {
    *ppv = this;
  }
  if (IID_IDropTarget == riid) {
    *ppv = (IDropTarget*) this;
  }
  if (NULL != *ppv) {
    ((LPUNKNOWN)*ppv)->AddRef();
    return NOERROR;
  }
  return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) IECtrl::DropTarget::AddRef(void) {
  ++m_cRef;
  return m_cRef;
}

STDMETHODIMP_(ULONG) IECtrl::DropTarget::Release(void) {
  --m_cRef;
  return m_cRef;
}

// IDropTarget
STDMETHODIMP IECtrl::DropTarget::DragEnter(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) {
  m_fAllowDrop = (QueryDataObject(pDataObject) && (m_pCtrl->m_pDropListener != NULL));

  if (m_fAllowDrop) {
    *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
    SetFocus(m_pCtrl->m_hWnd);
    // PositionCursor(m_hWnd, pt);
  } else {
    *pdwEffect = DROPEFFECT_NONE;
  }
  return S_OK;
}

STDMETHODIMP IECtrl::DropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) {
  if (m_fAllowDrop) {
    *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
    // PositionCursor(m_hWnd, pt);
  } else {
    *pdwEffect = DROPEFFECT_NONE;
  }
  return S_OK;
}

STDMETHODIMP IECtrl::DropTarget::DragLeave(void) {
  return S_OK;
}

STDMETHODIMP IECtrl::DropTarget::Drop(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) {
  // PositionCursor(m_hWnd, pt);
  if (m_fAllowDrop) {
    DropData(pDataObject);
    *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
  } else {
    *pdwEffect = DROPEFFECT_NONE;
  }
  return S_OK;
}

bool IECtrl::DropTarget::QueryDataObject(IDataObject *pDataObject) {
  // tylko drop plików CF_HDROP i TYMED_HGLOBAL
  FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
  return pDataObject->QueryGetData(&fmtetc) == S_OK ? true : false;
}

DWORD IECtrl::DropTarget::DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed) {
  // tu mozna zrobic rozroznienie po roznych parametrach
  return DROPEFFECT_COPY;
}

void IECtrl::DropTarget::DropData(IDataObject *pDataObject) {
  FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
  STGMEDIUM medium;

  if (SUCCEEDED(pDataObject->GetData(&fmtetc, &medium))) {
    if (medium.tymed == TYMED_HGLOBAL && medium.hGlobal) {
      DROPFILES * df = (DROPFILES *)GlobalLock(medium.hGlobal);
      SIZE_T lBufferSize = GlobalSize(medium.hGlobal);

      char * file = NULL;
      void * pFile = (BYTE*) df + df->pFiles;
      DropListener::tFiles files;

      if (df->fWide) {
        wchar_t* p_wFile = (wchar_t*)pFile;
        do {
          int len = wcslen(p_wFile)+1;
          file = new char[len];
          WideCharToMultiByte(CP_ACP, 0, p_wFile, len, file, len, NULL, FALSE);
          files.push_back((const char*)file);
          p_wFile += len;
        } while (p_wFile[0] != L'\0');
      } else {
        char* p_aFile = (char*)pFile;
        do {
          char* p_aFile = (char*)pFile;
          int len = strlen(p_aFile) + 1;
          file = new char[len];
          files.push_back((const char*)file);
          p_aFile += len;
        } while (p_aFile[0] != '\0');
      }
      if (m_pCtrl->m_pDropListener != NULL) {
        m_pCtrl->m_pDropListener->fileDropped(files, m_pCtrl);
      }
      for (DropListener::tFiles::iterator it = files.begin(); it != files.end(); it++) {
        delete [] (*it);
      }
      GlobalUnlock(medium.hGlobal);
    }
    ReleaseStgMedium(&medium);
  }
}

/*
 * IECtrl::iObject *
                   */

IECtrl::iObject::iObject(IECtrl* pCtrl, bool extModificate): m_cRef(0), m_pCtrl(pCtrl), _extModificate(extModificate) {
  setProperty("name", "[IECtrl::iObject]", attrReader);
}

IECtrl* IECtrl::iObject::getIECtrl() {
  return m_pCtrl;
}
void IECtrl::iObject::setIECtrl(IECtrl* pCtrl) {
  m_pCtrl = pCtrl;
}

IECtrl::iObject::~iObject() {
  ASSERT(m_cRef == 0);
}

STDMETHODIMP_(ULONG) IECtrl::iObject::AddRef(void) {
  ++m_cRef;
  return m_cRef;
}

STDMETHODIMP_(ULONG) IECtrl::iObject::Release(void) {
  --m_cRef;
  return m_cRef;
}

STDMETHODIMP IECtrl::iObject::GetTypeInfoCount(UINT *ptr) { 
  return E_NOTIMPL; 
}

STDMETHODIMP IECtrl::iObject::GetTypeInfo(UINT iTInfo, LCID lcid, LPTYPEINFO* ppTInfo) { 
  return S_OK; 
}

STDMETHODIMP IECtrl::iObject::Invoke(DISPID id, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdp, VARIANT* pvarRes, EXCEPINFO* pExcepInfo, UINT* puArgErr) {
  if (!pdp) return E_INVALIDARG;

  try {
    if (wFlags & DISPATCH_METHOD) {
      if (hasCallback(id)) {
        Var args;
        for (UINT i = 0; i < pdp->cArgs; i++) {
          args[-1] = pdp->rgvarg[pdp->cArgs - i - 1];
        }
        Var ret = trigger(id, args);
        ret.getVariant(pvarRes);
        return S_OK;
      }
    } else if (wFlags & DISPATCH_PROPERTYGET) {
      if (!id) {
        IECtrl::Var ret;
        if (hasCallback("toString")) {
          ret = trigger(getCallback("toString")->id, Var());
        } else if (hasProperty("name")) {
          ret = getProperty("name")->var;
        } else {
          return DISP_E_MEMBERNOTFOUND;
        }
        ret.getVariant(pvarRes);
        return S_OK;
      } else if (hasCallback(id)) {
        sCallback* method = getCallback(id);
        IECtrl::Var ret;

        if (method->getter) {
          ret = trigger(method->id, Var());
        } else {
          ret = true;
        }
        ret.getVariant(pvarRes);
        return S_OK;

      } else if (hasProperty(id)) {
        getProperty(id)->var.getVariant(pvarRes);
        return S_OK;
      }
    } else if ((wFlags & DISPATCH_PROPERTYPUT) || (wFlags & DISPATCH_PROPERTYPUTREF)) {
      sProperty* s = getProperty(id);
      if (s && s->attr > attrReader) {
        if (pdp->cArgs) {
          s->var = pdp->rgvarg[pdp->cArgs - 1];
          return S_OK;
        }
      }
    }
    return DISP_E_MEMBERNOTFOUND;
  } catch (const Exception& e) {
    pExcepInfo->bstrDescription = _com_util::ConvertStringToBSTR(e.getText().c_str());
    pExcepInfo->wCode = 1000;
    return DISP_E_EXCEPTION;
  }
}

STDMETHODIMP IECtrl::iObject::QueryInterface(REFIID riid, PVOID *ppv) {
  *ppv = NULL;
  if (IID_IUnknown == riid) {
    *ppv = this;
  }
  if (IID_IDispatch == riid) {
    *ppv = (IDispatch*) this;
  }
  if (IID_IDispatchEx == riid) {
    *ppv = (IDispatchEx*) this;
  }
  if (NULL != *ppv) {
    ((LPUNKNOWN)*ppv)->AddRef();
    return NOERROR;
  }
  return E_NOINTERFACE;
}

STDMETHODIMP IECtrl::iObject::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) { 
  HRESULT hr = NOERROR;

  for (UINT i = 0; i < cNames; i++) {
    const char* szName = _com_util::ConvertBSTRToString(rgszNames[i]);

    if (hasCallback(szName)) {
      rgDispId[i] = getCallback(szName)->id;
    } else if (hasProperty(szName)) {
      rgDispId[i] = getProperty(szName)->id;
    } else {
      hr = ResultFromScode(DISP_E_UNKNOWNNAME);
      rgDispId[i] = DISPID_UNKNOWN;
    }
    delete [] szName;
  }
  return hr;
}

IECtrl::Var IECtrl::iObject::trigger(long id, IECtrl::Var& args) {
  sCallback* f = getCallback(id);
  return !f ? IECtrl::Var() : f->signal(args, this);
}

void IECtrl::iObject::bindMethod(const string& name, fCallback f, bool getter) {
  if (!getCallback(name)) {
    _callbacks.push_back(new sCallback(name, f, getter));
  }
}

bool IECtrl::iObject::unbindMethod(const string& name) {
  for (tCallbacks::iterator it = _callbacks.begin(); it != _callbacks.end(); it++) {
    if ((*it)->name != name) continue;

    delete *it;
    _callbacks.erase(it);
    return true;
  }
  return false;
}

bool IECtrl::iObject::hasCallback(long id) {
  return getCallback(id) > 0;
}

bool IECtrl::iObject::hasCallback(const string& name) {
  return getCallback(name) > 0;
}

IECtrl::iObject::sCallback* IECtrl::iObject::getCallback(const string& name) {
  for (tCallbacks::iterator it = _callbacks.begin(); it != _callbacks.end(); it++) {
    if ((*it)->name == name) return *it;
  }
  return 0;
}

IECtrl::iObject::sCallback* IECtrl::iObject::getCallback(long id) {
  for (tCallbacks::iterator it = _callbacks.begin(); it != _callbacks.end(); it++) {
    if ((*it)->id == id) return *it;
  }
  return 0;
}

bool IECtrl::iObject::hasProperty(const string& name) {
  return getProperty(name) > 0;
}

bool IECtrl::iObject::hasProperty(long id) {
  return getProperty(id) > 0;
}

IECtrl::iObject::sProperty* IECtrl::iObject::getProperty(const string& name) {
  for (tProperties::iterator it = _properties.begin(); it != _properties.end(); it++) {
    if ((*it)->name == name) return *it;
  }
  return 0;
}

IECtrl::iObject::sProperty* IECtrl::iObject::getProperty(long id) {
  for (tProperties::iterator it = _properties.begin(); it != _properties.end(); it++) {
    if ((*it)->id == id) return *it;
  }
  return 0;
}

IECtrl::iObject::sProperty* IECtrl::iObject::setProperty(const string& name, Var v, enAttributes attr) {
  sProperty* val;
  
  if (!hasProperty(name)) {
    _properties.push_back(val = new sProperty(name, attr));
  } else {
    val = getProperty(name);
    val->attr = attr;
  }
  val->var = v;
  return val;
}

STDMETHODIMP IECtrl::iObject::InvokeEx(DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp, VARIANT *pvarRes, EXCEPINFO *pei, IServiceProvider *pspCaller) { 
  return Invoke(id, IID_IDispatch, lcid, wFlags, pdp, pvarRes, pei, 0);
}

STDMETHODIMP IECtrl::iObject::GetDispID(BSTR bstrName, DWORD grfdex, DISPID *pid) {
  const char* name = _com_util::ConvertBSTRToString(bstrName);
  long id = 0;

  if (hasCallback(name)) {
    id = getCallback(name)->id;
  } else if (hasProperty(name)) {
    id = getProperty(name)->id;
  } else {
    if (grfdex & fdexNameEnsure && _extModificate) {
      sProperty* prop = setProperty(name, Var(), attrAccessor);

      prop->external = true;
      id = prop->id;
    }
  }

  *pid = id;
  delete [] name;

  return id ? S_OK : DISP_E_UNKNOWNNAME;
}

STDMETHODIMP IECtrl::iObject::DeleteMemberByName(BSTR bstrName, DWORD grfdex) {
  const char* name = _com_util::ConvertBSTRToString(bstrName);
  sProperty* v = getProperty(name);
  bool deleted = false;

  if (_extModificate && v && v->external) {
    for (tProperties::iterator it = _properties.begin(); it != _properties.end(); it++) {
      if ((*it)->name == name) {
        delete *it;
        _properties.erase(it);
        deleted = true;
        break;
      }
    }
  }
  delete [] name;

  return deleted ? S_OK : S_FALSE;
}

STDMETHODIMP IECtrl::iObject::DeleteMemberByDispID(DISPID id) {
  sProperty* v = getProperty(id);

  if (_extModificate && v && v->external) {
    for (tProperties::iterator it = _properties.begin(); it != _properties.end(); it++) {
      if ((*it)->id == id) {
        delete *it;
        _properties.erase(it);
        return S_OK;
      }
    }
  }
  return S_FALSE;
}

STDMETHODIMP IECtrl::iObject::GetMemberProperties(DISPID id, DWORD grfdexFetch, DWORD *pgrfdex) {
  DWORD grfdexPCA = 0, grfdexPCNA = 0, grfdexPEA = 0;
  bool exists = true;

  if (hasCallback(id)) {
    grfdexPCA = fdexPropCanGet | fdexPropCanCall;
    grfdexPCNA = fdexPropCannotPutRef | fdexPropCannotPut;
    grfdexPEA = fdexPropDynamicType;

  } else if (hasProperty(id)) {
    grfdexPCA = fdexPropCanGet | fdexPropCanPut;
    grfdexPCNA = fdexPropCannotCall | fdexPropCannotConstruct | fdexPropCannotPutRef;
    grfdexPEA = fdexPropDynamicType;

    if (getProperty(id)->attr < attrWriter) {
      grfdexPCNA |= fdexPropCannotPut;
      grfdexPCA ^= fdexPropCanPut;
    }
  } else {
    exists = false;
  }

  if (grfdexFetch == grfdexPropCanAll) {
    *pgrfdex = grfdexPCA;
  }  else if (grfdexFetch == grfdexPropCannotAll) {
    *pgrfdex = grfdexPCNA;
  } else if (grfdexFetch == grfdexPropExtraAll) {
    *pgrfdex = grfdexPEA;
  } else if (grfdexFetch == grfdexPropAll) {
    *pgrfdex = grfdexPCA | grfdexPCNA | grfdexPEA;
  } else {
    *pgrfdex = 0;
  }
  return exists ? S_OK : DISP_E_UNKNOWNNAME;
}

STDMETHODIMP IECtrl::iObject::GetMemberName(DISPID id, BSTR *pbstrName) {
  string name;

  if (hasCallback(id)) {
    name = getCallback(id)->name;
  } else if (hasProperty(id)) {
    name = getProperty(id)->name;
  }
  if (!name.length()) {
    return DISP_E_UNKNOWNNAME;
  }
  *pbstrName = _com_util::ConvertStringToBSTR(name.c_str());
  return S_OK;
}

STDMETHODIMP IECtrl::iObject::GetNextDispID(DWORD grfdex, DISPID id, DISPID *pid) {
  if (id == DISPID_STARTENUM) {
    if (_callbacks.size() > 0) {
        *pid = _callbacks[0]->id;
        return S_OK;
      } else if (_properties.size() > 0) {
        *pid = _properties[0]->id;
        return S_OK;
      }
  } else {
    bool next = false;
    for (tCallbacks::iterator it = _callbacks.begin(); it != _callbacks.end(); it++) {
      if ((*it)->id == id) {
        next  = true;
      } else if (next) {
        *pid = (*it)->id;
        return S_OK;
      }
    }
    for (tProperties::iterator it = _properties.begin(); it != _properties.end(); it++) {
      if ((*it)->id == id) {
        next = true;
      } else if (next) {
        *pid = (*it)->id;
        return S_OK;
      }
    }
  }

  *pid = 0;
  return S_FALSE;
}

STDMETHODIMP IECtrl::iObject::GetNameSpaceParent(IUnknown **ppunk) {
  *ppunk = NULL;
  return S_FALSE;
}
