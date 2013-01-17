/*
 *  KernelEx
 *  Copyright 1995 Martin von Loewis
 *  Copyright 1996 Alexandre Julliard
 *  Copyright 2009-2010 Tihiy
 *
 *  This file is part of KernelEx source code.
 *
 *  KernelEx is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation; version 2 of the License.
 *
 *  KernelEx is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <windows.h>
#include "thuni_layer.h"
#include "thuni_macro.h"
#include "thuni_thunk.h"

WPARAM wparam_AtoW( HWND hwnd, UINT message, WPARAM wParam, BOOL messDBCS )
{
	WPARAM newwParam = wParam;
	switch(message)
	{
	case WM_CHAR:
		/* WM_CHAR is magic: a DBCS char can be sent/posted as two consecutive WM_CHAR
		 * messages, in which case the first char is stored, and the conversion
		 * to Unicode only takes place once the second char is sent/posted.
		 */
		if (messDBCS)
		{
			if ( wParam>127 && wParam<256 )
			{
				UCHAR charsA[2];
				int ch = 0;
				UINT codepage = GetCurrentKeyboardCP();
				BYTE firstchar = (BYTE)GetPropA(hwnd, c_szDBCSProp);
				if ( firstchar ) //first dbcs char stored?
				{
					ch = 1;
					charsA[0] = firstchar;
					RemovePropA( hwnd, c_szDBCSProp );
				}
				else
				{
					if ( IsDBCSLeadByteEx(codepage, wParam) )
					{
						SetPropA( hwnd, c_szDBCSProp, (HANDLE)wParam );  //store first char
						return 0L;
					}					
				}
				charsA[ch] = wParam;
				ch++;
				MultiByteToWideChar( codepage, 0, (LPSTR)charsA, ch, (LPWSTR)&newwParam, 1 );
			}
			return newwParam;
		}
		/* else fall through */
	case WM_CHARTOITEM:
	case EM_SETPASSWORDCHAR:
	case WM_DEADCHAR:
	case WM_SYSCHAR:
	case WM_SYSDEADCHAR:
	case WM_MENUCHAR:
		{
			if (wParam>127 && wParam<256) 
				MultiByteToWideChar( GetCurrentKeyboardCP(), 0, (LPSTR)&wParam, 2, (LPWSTR)&newwParam, 1 );
			break;
		}
	case WM_IME_CHAR:
		{
			UCHAR dbcs[2];
			dbcs[0]=HIBYTE(wParam);
			dbcs[1]=LOBYTE(wParam);
			MultiByteToWideChar( GetCurrentKeyboardCP(), 0, (LPSTR)dbcs, 2, (LPWSTR)&newwParam, 1 );
			break;
		}
	}
	return newwParam;
}

WPARAM wparam_WtoA( UINT message, WPARAM wParam )
{
	WPARAM newwParam = wParam & 0xFFFF00FF;
	switch(message)
	{
		case WM_CHAR:
		case WM_CHARTOITEM:
		case EM_SETPASSWORDCHAR:
		case WM_DEADCHAR:
		case WM_SYSCHAR:
		case WM_SYSDEADCHAR:
		case WM_MENUCHAR:
		{
			BOOL fail;
			if (wParam>127)
				WideCharToMultiByte( GetCurrentKeyboardCP(), WC_NO_BEST_FIT_CHARS, (LPWSTR)&wParam, 1, (LPSTR)&newwParam, 2, NULL, &fail );
			if (fail) return wParam;			
			break;
		}
		case WM_IME_CHAR:
		{
			WideCharToMultiByte( GetCurrentKeyboardCP(), 0, (LPWSTR)&wParam, 1, (LPSTR)&newwParam, 2, NULL, NULL );
			newwParam = MAKEWPARAM(MAKEWORD(HIBYTE(newwParam),LOBYTE(newwParam)),HIWORD(newwParam));
			break;
		}
	}
	return newwParam;
}


/**********************************************************************
 * Wine	WINPROC_TestLBForStr
 *
 * Return TRUE if the lparam is a string
 */
static BOOL TestLBForStr( HWND hwnd, UINT msg )
{
	DWORD style = GetWindowLongA( hwnd, GWL_STYLE );
	if (msg <= CB_MSGMAX)
		return (!(style & (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE)) || (style & CBS_HASSTRINGS));
	else
		return (!(style & (LBS_OWNERDRAWFIXED | LBS_OWNERDRAWVARIABLE)) || (style & LBS_HASSTRINGS));

}

/**********************************************************************
* Call an Ansi window procedure, translating args from Unicode to Ansi.
* 9x control wise, CallWindowProcA must be used instead of direct call.
*/
LRESULT WINAPI CallProcAnsiWithUnicode( WNDPROC callback, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	switch (msg)
	{
		case WM_CREATE:
		case WM_NCCREATE:
			{
				if (!VALID_PTR(lParam)) return CallWindowProcA(callback,hwnd,msg,wParam,lParam);
				
				LPCREATESTRUCTW csW = (LPCREATESTRUCTW)lParam;
				CREATESTRUCTA csA;
				LPMDICREATESTRUCTW mcsW = (LPMDICREATESTRUCTW)csW->lpCreateParams;
				MDICREATESTRUCTA mcsA; 
				
				CopyMemory(&csA, csW, sizeof(CREATESTRUCT));
				STACK_WtoA(csW->lpszName,csA.lpszName);
				STACK_WtoA(csW->lpszClass,csA.lpszClass);
							
				if ( GetWindowLongA(hwnd, GWL_EXSTYLE) & WS_EX_MDICHILD && VALID_PTR(mcsW) )
				{
					//mmm test, i can't understand Wine
					CopyMemory(&mcsA, mcsW, sizeof(MDICREATESTRUCT));
					STACK_WtoA(mcsW->szClass,mcsA.szClass);
					STACK_WtoA(mcsW->szTitle,mcsA.szTitle);					
					csA.lpCreateParams = &mcsA;
				}
				return CallWindowProcA(callback,hwnd,msg,wParam,(LPARAM)&csA);
			}
		case WM_MDICREATE:
			{
				if (!VALID_PTR(lParam)) return CallWindowProcA(callback,hwnd,msg,wParam,lParam);
				
				LPMDICREATESTRUCTW mcsW = (LPMDICREATESTRUCTW)lParam;
				MDICREATESTRUCTA mcsA;
				CopyMemory(&mcsA, mcsW, sizeof(MDICREATESTRUCT));
				STACK_WtoA(mcsW->szClass,mcsA.szClass);
				STACK_WtoA(mcsW->szTitle,mcsA.szTitle);
				return CallWindowProcA(callback,hwnd,msg,wParam,(LPARAM)&mcsA);
			}
		case LB_GETTEXT:
		case CB_GETLBTEXT:
			if (lParam && TestLBForStr( hwnd, msg ))
			{
				LPSTR textA;
				int len = CallWindowProcA( callback, hwnd, msg == LB_GETTEXT ? LB_GETTEXTLEN : CB_GETLBTEXTLEN, wParam, 0 );				
				ABUFFER_ALLOC(textA,len); //note that len does not include null
				LRESULT ret = CallWindowProcA( callback, hwnd, msg, wParam,(LPARAM)textA );
				ABUFFER_toW(textA,lParam,len+1);
				BUFFER_FREE(textA);
				return ret;
			}			
			break;
		case WM_ASKCBFORMATNAME:
		case WM_GETTEXT:
			{
				if ( !lParam ) return 0L;
				if ( !wParam ) //no text
				{
					*(LPWSTR)(lParam) = 0;
					return 0L;
				}			
				LPSTR textA;
				ABUFFER_ALLOC(textA,wParam);
				LRESULT ret = CallWindowProcA( callback, hwnd, msg, wParam,(LPARAM)textA );
				ABUFFER_toW(textA,lParam,wParam);
				BUFFER_FREE(textA);
				return ret;
			}
		case EM_GETLINE:
			{
				WORD len = *(WORD *)lParam;
				LPSTR textA;
				ABUFFER_ALLOC(textA,len);
				*(WORD *)textA = len;
				LRESULT ret = CallWindowProcA( callback, hwnd, msg, wParam,(LPARAM)textA );
				ABUFFER_toW(textA,lParam,len);
				BUFFER_FREE(textA);
				return ret;				
			}
		case LB_ADDSTRING:
		case LB_INSERTSTRING:
		case LB_FINDSTRING:
		case LB_FINDSTRINGEXACT:
		case LB_SELECTSTRING:
		case CB_ADDSTRING:
		case CB_INSERTSTRING:
		case CB_FINDSTRING:
		case CB_FINDSTRINGEXACT:
		case CB_SELECTSTRING:
			if ( !lParam || !TestLBForStr( hwnd, msg ) ) break;
		/* fall through */
		case WM_WININICHANGE:		
		case WM_DEVMODECHANGE:
			if ( IsBadReadPtr((LPWSTR)lParam, sizeof(WCHAR)) ) break; //i don't trust those two!!
		case CB_DIR:
		case LB_DIR:
		case LB_ADDFILE:
		case EM_REPLACESEL:		
		case WM_SETTEXT:
		{
			LPSTR textA;
			STACK_WtoA( lParam, textA );
			return CallWindowProcA(callback,hwnd,msg,wParam,(LPARAM)textA);
		}
		case WM_CHAR:
		{
			if ( !HIWORD(wParam) && LOWORD(wParam)>127 )
			{
				LRESULT ret;
				UCHAR charsA[2]; //don't use char!
				BOOL fail = FALSE;				
				int ch = WideCharToMultiByte( GetCurrentKeyboardCP(), WC_NO_BEST_FIT_CHARS, (LPWSTR)&wParam, 1, (LPSTR)charsA, 2, NULL, &fail );
				if ( fail ) break; //can't translate, game over
				ret = CallWindowProcA(callback,hwnd,msg,charsA[0],lParam);
				if ( ch==2 ) CallWindowProcA(callback,hwnd,msg,charsA[1],lParam);
				return ret;
			}
			break;
		}
		case WM_CHARTOITEM:
		case EM_SETPASSWORDCHAR:
		case WM_DEADCHAR:
		case WM_SYSCHAR:
		case WM_SYSDEADCHAR:
		case WM_MENUCHAR:
		case WM_IME_CHAR:
			wParam = wparam_WtoA( msg, wParam );
		break;
		case WM_NOTIFYFORMAT:
			/* for default window procedures */
			if ( IS_SHARED(callback) && lParam == NF_QUERY && IsWindowUnicode_NEW((HWND)wParam) )
				return NFR_UNICODE;
	}
	return CallWindowProcA(callback,hwnd,msg,wParam,lParam);
}

/**********************************************************************
* Call an Unicode window procedure, translating args from Ansi to Unicode.
* No need to use CallWindowProcA since callback can't be 16-bit thunk, 
* but using CallWindowProc_stdcall since we adapt to buggy W functions.
*/
LRESULT WINAPI CallProcUnicodeWithAnsi( WNDPROC callback, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{		
		case WM_NCCREATE: 
			/* unicode window is being created? mark it before event fires */
			if (IsWindowReallyUnicode(hwnd))
					SetWindowUnicode(hwnd,TRUE);
			//fall down
		case WM_CREATE:
			{
				if (!VALID_PTR(lParam)) return CallWindowProc_stdcall(callback,hwnd,msg,wParam,lParam);
				
				LPCREATESTRUCTA csA = (LPCREATESTRUCTA)lParam;
				CREATESTRUCTW csW;
				LPMDICREATESTRUCTA mcsA = (LPMDICREATESTRUCTA)csA->lpCreateParams;
				MDICREATESTRUCTW mcsW;
				
				CopyMemory(&csW, csA, sizeof(CREATESTRUCT));
				STACK_AtoW(csA->lpszName,csW.lpszName);
				STACK_AtoW(csA->lpszClass,csW.lpszClass);
							
				if ( GetWindowLongA(hwnd, GWL_EXSTYLE) & WS_EX_MDICHILD && VALID_PTR(mcsA) )
				{
					CopyMemory(&mcsW, mcsA, sizeof(MDICREATESTRUCT));
					STACK_AtoW(mcsA->szClass,mcsW.szClass);
					STACK_AtoW(mcsA->szTitle,mcsW.szTitle);					
					csW.lpCreateParams = &mcsW;
				}
				return CallWindowProc_stdcall(callback,hwnd,msg,wParam,(LPARAM)&csW);
			}
		case WM_MDICREATE:
			{
				if (!VALID_PTR(lParam)) return CallWindowProc_stdcall(callback,hwnd,msg,wParam,lParam);

				LPMDICREATESTRUCTA mcsA = (LPMDICREATESTRUCTA)lParam;
				MDICREATESTRUCTW mcsW;
				CopyMemory(&mcsW, mcsA, sizeof(MDICREATESTRUCT));
				STACK_AtoW(mcsA->szClass,mcsW.szClass);
				STACK_AtoW(mcsA->szTitle,mcsW.szTitle);
				return CallWindowProc_stdcall(callback,hwnd,msg,wParam,(LPARAM)&mcsW);
			}
		case LB_GETTEXT:
		case CB_GETLBTEXT:
			if (lParam && TestLBForStr( hwnd, msg ))
			{
				LPWSTR textW;
				int len = CallWindowProc_stdcall( callback,hwnd, msg == LB_GETTEXT ? LB_GETTEXTLEN : CB_GETLBTEXTLEN, wParam, 0 );				
				WBUFFER_ALLOC(textW,len);
				LRESULT ret = CallWindowProc_stdcall( callback, hwnd, msg, wParam,(LPARAM)textW );
				WBUFFER_toA(textW, lParam, len+1);
				BUFFER_FREE(textW);
				return ret;
			}
			break;
		case WM_ASKCBFORMATNAME:
		case WM_GETTEXT:
			{
				if ( !lParam ) return 0L;
				if ( !wParam ) //no text
				{
					*(LPWSTR)(lParam) = 0;
					return 0L;
				}
				LPWSTR textW;
				WBUFFER_ALLOC(textW,wParam);
				LRESULT ret = CallWindowProc_stdcall( callback, hwnd, msg, wParam,(LPARAM)textW );
				WBUFFER_toA(textW,lParam,wParam);
				BUFFER_FREE(textW);
				return ret;
			}
		case EM_GETLINE:
			{
				WORD len = *(WORD *)lParam;
				LPWSTR textW;
				WBUFFER_ALLOC(textW,len);
				*(WORD *)textW = len;
				LRESULT ret = CallWindowProc_stdcall( callback, hwnd, msg, wParam,(LPARAM)textW );
				WBUFFER_toA(textW,lParam,len);
				BUFFER_FREE(textW);
				return ret;				
			}			
		case LB_ADDSTRING:
		case LB_INSERTSTRING:
		case LB_FINDSTRING:
		case LB_FINDSTRINGEXACT:
		case LB_SELECTSTRING:
		case CB_ADDSTRING:
		case CB_INSERTSTRING:
		case CB_FINDSTRING:
		case CB_FINDSTRINGEXACT:
		case CB_SELECTSTRING:
			if ( !lParam || !TestLBForStr( hwnd, msg ) ) break;
		/* fall through */
		case WM_WININICHANGE:		
		case WM_DEVMODECHANGE:
			if ( IsBadReadPtr((LPSTR)lParam, sizeof(CHAR)) ) break; //i don't trust those two!!
		case CB_DIR:
		case LB_DIR:
		case LB_ADDFILE:
		case EM_REPLACESEL:		
		case WM_SETTEXT:
		{
			LPWSTR textW;
			STACK_AtoW( lParam, textW );
			return CallWindowProc_stdcall(callback,hwnd,msg,wParam,(LPARAM)textW);
		}
		case WM_CHAR:									
		case WM_CHARTOITEM:
		case EM_SETPASSWORDCHAR:
		case WM_DEADCHAR:
		case WM_SYSCHAR:
		case WM_SYSDEADCHAR:
		case WM_MENUCHAR:
		case WM_IME_CHAR:
		{
			wParam = wparam_AtoW( hwnd, msg, wParam, TRUE );
		}			
		break;				
	}
	return CallWindowProc_stdcall(callback,hwnd,msg,wParam,lParam);	
}
