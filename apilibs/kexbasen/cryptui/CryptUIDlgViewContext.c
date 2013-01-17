/*
 *  KernelEx
 *  Copyright 2010, Tihiy
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
#include <wintrust.h>

typedef struct {
	DWORD            dwSize;
	HWND             hwndParent;
	DWORD            dwFlags;
	LPCWSTR          szTitle;
	PCCTL_CONTEXT    pCtlContext;
	DWORD            unknown[6];
} CRYPTUI_VIEWCTL_STRUCTW; //size = 0x2C

typedef struct {
	DWORD            dwSize;
	HWND             hwndParent;
	DWORD            dwFlags;
	LPCWSTR          szTitle;
	PCCRL_CONTEXT    pCtlContext;
	DWORD            unknown[4];
} CRYPTUI_VIEWCRL_STRUCTW; //size = 0x24

typedef struct tagCRYPTUI_VIEWCERTIFICATE_STRUCTW {
    DWORD            dwSize;
    HWND             hwndParent;
    DWORD            dwFlags;
    LPCWSTR          szTitle;
    PCCERT_CONTEXT   pCertContext;
    LPCSTR*          rgszPurposes;
    DWORD            cPurposes;
    union
    {
		CRYPT_PROVIDER_DATA const* pCryptProviderData;
		HANDLE                     hWVTStateData;
    };
    BOOL             fpCryptProviderDataTrustedUsage;
    DWORD            idxSigner;
    DWORD            idxCert;
    BOOL             fCounterSigner;
    DWORD            idxCounterSigner;
    DWORD            cStores;
    HCERTSTORE*      rghStores;
    DWORD            cPropSheetPages;
    LPCPROPSHEETPAGEW rgPropSheetPages;
    DWORD            nStartPage;
} CRYPTUI_VIEWCERTIFICATE_STRUCTW,*PCRYPTUI_VIEWCERTIFICATE_STRUCTW;
typedef const CRYPTUI_VIEWCERTIFICATE_STRUCTW *PCCRYPTUI_VIEWCERTIFICATE_STRUCTW;

#ifdef __cplusplus
extern "C" {
#endif
BOOL WINAPI CryptUIDlgViewCertificateW(PCCRYPTUI_VIEWCERTIFICATE_STRUCTW, BOOL*);
BOOL WINAPI CryptUIDlgViewCTLW(CRYPTUI_VIEWCTL_STRUCTW*);
BOOL WINAPI CryptUIDlgViewCRLW(CRYPTUI_VIEWCRL_STRUCTW*);
#ifdef __cplusplus
}
#endif

/* MAKE_EXPORT CryptUIDlgViewContext_new=CryptUIDlgViewContext */
BOOL WINAPI CryptUIDlgViewContext_new( DWORD dwContextType, const void *pvContext,
  HWND hwnd, LPCWSTR pwszTitle, DWORD dwFlags, void *pvReserved)
{
	switch (dwContextType) {
	case CERT_STORE_CERTIFICATE_CONTEXT:
		{
			CRYPTUI_VIEWCERTIFICATE_STRUCTW certW;
			memset(&certW,0,sizeof(certW));
			certW.dwSize = sizeof(certW);
			certW.hwndParent = hwnd;
			certW.szTitle = pwszTitle;
			certW.pCertContext = (PCERT_CONTEXT)pvContext;
			return CryptUIDlgViewCertificateW(&certW,NULL);
		}
		break;
	case CERT_STORE_CTL_CONTEXT:
		{
			CRYPTUI_VIEWCTL_STRUCTW ctlW;
			memset(&ctlW,0,sizeof(ctlW));
			ctlW.dwSize = sizeof(ctlW);
			ctlW.hwndParent = hwnd;
			ctlW.szTitle = pwszTitle;
			ctlW.pCtlContext = (PCCTL_CONTEXT)pvContext;
			return CryptUIDlgViewCTLW(&ctlW);
		}
		break;
	case CERT_STORE_CRL_CONTEXT:
		{
			CRYPTUI_VIEWCRL_STRUCTW crlW;
			memset(&crlW,0,sizeof(crlW));
			crlW.dwSize = sizeof(crlW);
			crlW.hwndParent = hwnd;
			crlW.szTitle = pwszTitle;
			crlW.pCtlContext = (PCCRL_CONTEXT)pvContext;
			return CryptUIDlgViewCRLW(&crlW);
		}
		break;
	default:
		return FALSE;
	}
}
