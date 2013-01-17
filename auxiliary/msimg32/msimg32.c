/*
 *  KernelEx
 *  Copyright (C) 2008, Tihiy
 *  Copyright 1993, 1994 Alexandre Julliard
 *  Copyright 1997 Bertho A. Stultiens
 *            1999 Huw D M Davies
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
#define INITBITMAPINFO(bInfo,cx,cy) \
	ZeroMemory(&bInfo,sizeof(BITMAPINFO)); \
	bInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); \
	bInfo.bmiHeader.biWidth = cx; \
	bInfo.bmiHeader.biHeight = cy; \
	bInfo.bmiHeader.biPlanes = 1; \
	bInfo.bmiHeader.biBitCount = 32;

#ifndef GRADIENT_FILL_RECT_H
#define GRADIENT_FILL_RECT_H 0x00
#define GRADIENT_FILL_RECT_V 0x01
#define GRADIENT_FILL_TRIANGLE 0x02
#define GRADIENT_FILL_OP_FLAG 0xff
#endif

#ifndef AC_SRC_ALPHA
#define AC_SRC_ALPHA 0x01
#endif
	
BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		DisableThreadLibraryCalls(hinstDLL);
	return TRUE;
}


BOOL WINAPI AlphaBlend_NEW( HDC hdcDest,  // handle to destination DC
	int nXOriginDest,            // x-coord of upper-left corner
	int nYOriginDest,            // y-coord of upper-left corner
	int nWidthDest,              // destination width
	int nHeightDest,             // destination height
	HDC hdcSrc,                  // handle to source DC
	int nXOriginSrc,             // x-coord of upper-left corner
	int nYOriginSrc,             // y-coord of upper-left corner
	int nWidthSrc,               // source width
	int nHeightSrc,              // source height
	BLENDFUNCTION blendFunction  // alpha-blending function
)
{
	unsigned int srcalpha, dstalpha;
	BITMAPINFO bmi;
	HBITMAP srcBM, dstBM, dcBM;
	HDC srcDC, dstDC;
	RGBQUAD *srcPixel, *dstPixel;	
	INITBITMAPINFO(bmi,nWidthDest,nHeightDest);

	if ( !hdcDest || !hdcSrc || blendFunction.BlendOp != AC_SRC_OVER 
		|| nWidthDest < 0 || nHeightDest < 0 || nWidthSrc < 0 || nHeightSrc < 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	if ( !blendFunction.SourceConstantAlpha ) return TRUE; //nothing to do
	if ( !(blendFunction.AlphaFormat & AC_SRC_ALPHA) && blendFunction.SourceConstantAlpha == 0xFF ) //no alpha work
		return StretchBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, SRCCOPY );
		
	srcBM = CreateDIBSection(hdcSrc, &bmi, DIB_RGB_COLORS, (void*)&srcPixel, NULL, 0);
	dstBM = CreateDIBSection(hdcDest, &bmi, DIB_RGB_COLORS, (void*)&dstPixel, NULL, 0);
	
	if ( !srcBM || !dstBM )
	{
		DeleteObject(srcBM);
		DeleteObject(dstBM);
		return FALSE;
	}	
	//create a copy of source image
	srcDC = CreateCompatibleDC(hdcSrc);
	dcBM = SelectObject(srcDC, srcBM);
	StretchBlt(srcDC, 0, 0, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, SRCCOPY);
	SelectObject(srcDC, dcBM);
	DeleteDC(srcDC); //don't need no more
	//create a copy of dest image
	dstDC = CreateCompatibleDC(hdcDest);
	dcBM = SelectObject(dstDC, dstBM);
	BitBlt(dstDC, 0, 0, nWidthDest, nHeightDest, hdcDest, nXOriginDest, nYOriginDest, SRCCOPY);		
	//workwork
	if ( !(blendFunction.AlphaFormat & AC_SRC_ALPHA) ) //no alpha channel
	{
		int i;
		srcalpha = blendFunction.SourceConstantAlpha;
		dstalpha = 255 - srcalpha;
		for (i = 0; i < (nWidthDest*nHeightDest); i++)
		{
			dstPixel->rgbBlue = ( (srcPixel->rgbBlue * srcalpha) + (dstPixel->rgbBlue * dstalpha) ) / 255;
			dstPixel->rgbGreen = ( (srcPixel->rgbGreen * srcalpha) + (dstPixel->rgbGreen * dstalpha) ) / 255;
			dstPixel->rgbRed = ( (srcPixel->rgbRed * srcalpha) + (dstPixel->rgbRed * dstalpha) ) / 255;
			dstPixel->rgbReserved = ( (srcPixel->rgbReserved * srcalpha) + (dstPixel->rgbReserved * dstalpha) ) / 255;
			srcPixel++;
			dstPixel++;
		}
	}
	else
	{
		int i;
		unsigned int tmp;
		srcalpha = blendFunction.SourceConstantAlpha;
		for (i = 0; i < (nWidthDest*nHeightDest); i++)
		{
			dstalpha = 255 - (srcPixel->rgbReserved * srcalpha / 255);
			tmp = ((srcPixel->rgbRed * srcalpha) + (dstPixel->rgbRed * dstalpha)) / 255;
			if (tmp > 255) tmp = 255;
			dstPixel->rgbRed = tmp;
			tmp = ((srcPixel->rgbGreen * srcalpha) + (dstPixel->rgbGreen * dstalpha)) / 255;
			if (tmp > 255) tmp = 255;
			dstPixel->rgbGreen = tmp;
			tmp = ((srcPixel->rgbBlue * srcalpha) + (dstPixel->rgbBlue * dstalpha)) / 255;
			if (tmp > 255) tmp = 255;
			dstPixel->rgbBlue = tmp;			
			tmp = ((srcPixel->rgbReserved * srcalpha) + (dstPixel->rgbReserved * dstalpha)) / 255;
			if (tmp > 255) tmp = 255;
			dstPixel->rgbReserved = tmp;
			srcPixel++;
			dstPixel++;
		}		
	}
	BitBlt(hdcDest,nXOriginDest,nYOriginDest,nWidthDest,nHeightDest,dstDC,0,0,SRCCOPY);
	//destroy stuff we used
	SelectObject(dstDC, dcBM);
	DeleteDC(dstDC);
	DeleteObject(srcBM);
	DeleteObject(dstBM);

	return TRUE;
}

/******************************************************************************
 *           GdiGradientFill   (GDI32.@)
 *
 *  FIXME: we don't support the Alpha channel properly
 */
BOOL WINAPI GradientFill_NEW( HDC hdc, TRIVERTEX *vert_array, ULONG nvert,
                          void * grad_array, ULONG ngrad, ULONG mode )
{
  unsigned int i;

  if ( !hdc || !vert_array || ! grad_array )
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

  switch(mode) 
    {
    case GRADIENT_FILL_RECT_H:
      for(i = 0; i < ngrad; i++) 
        {
          GRADIENT_RECT *rect = ((GRADIENT_RECT *)grad_array) + i;
          TRIVERTEX *v1 = vert_array + rect->UpperLeft;
          TRIVERTEX *v2 = vert_array + rect->LowerRight;
          int y1 = v1->y < v2->y ? v1->y : v2->y;
          int y2 = v2->y > v1->y ? v2->y : v1->y;
          int x, dx;
          if (v1->x > v2->x)
            {
              TRIVERTEX *t = v2;
              v2 = v1;
              v1 = t;
            }
          dx = v2->x - v1->x;
          for (x = 0; x < dx; x++)
            {
              POINT pts[2];
              HPEN hPen, hOldPen;
              
              hPen = CreatePen( PS_SOLID, 1, RGB(
                  (v1->Red   * (dx - x) + v2->Red   * x) / dx >> 8,
                  (v1->Green * (dx - x) + v2->Green * x) / dx >> 8,
                  (v1->Blue  * (dx - x) + v2->Blue  * x) / dx >> 8));
              hOldPen = SelectObject( hdc, hPen );
              pts[0].x = v1->x + x;
              pts[0].y = y1;
              pts[1].x = v1->x + x;
              pts[1].y = y2;
              Polyline( hdc, &pts[0], 2 );
              DeleteObject( SelectObject(hdc, hOldPen ) );
            }
        }
      break;
    case GRADIENT_FILL_RECT_V:
      for(i = 0; i < ngrad; i++) 
        {
          GRADIENT_RECT *rect = ((GRADIENT_RECT *)grad_array) + i;
          TRIVERTEX *v1 = vert_array + rect->UpperLeft;
          TRIVERTEX *v2 = vert_array + rect->LowerRight;
          int x1 = v1->x < v2->x ? v1->x : v2->x;
          int x2 = v2->x > v1->x ? v2->x : v1->x;
          int y, dy;
          if (v1->y > v2->y)
            {
              TRIVERTEX *t = v2;
              v2 = v1;
              v1 = t;
            }
          dy = v2->y - v1->y;
          for (y = 0; y < dy; y++)
            {
              POINT pts[2];
              HPEN hPen, hOldPen;
              
              hPen = CreatePen( PS_SOLID, 1, RGB(
                  (v1->Red   * (dy - y) + v2->Red   * y) / dy >> 8,
                  (v1->Green * (dy - y) + v2->Green * y) / dy >> 8,
                  (v1->Blue  * (dy - y) + v2->Blue  * y) / dy >> 8));
              hOldPen = SelectObject( hdc, hPen );
              pts[0].x = x1;
              pts[0].y = v1->y + y;
              pts[1].x = x2;
              pts[1].y = v1->y + y;
              Polyline( hdc, &pts[0], 2 );
              DeleteObject( SelectObject(hdc, hOldPen ) );
            }
        }
      break;
    case GRADIENT_FILL_TRIANGLE:
      for (i = 0; i < ngrad; i++)  
        {
          GRADIENT_TRIANGLE *tri = ((GRADIENT_TRIANGLE *)grad_array) + i;
          TRIVERTEX *v1 = vert_array + tri->Vertex1;
          TRIVERTEX *v2 = vert_array + tri->Vertex2;
          TRIVERTEX *v3 = vert_array + tri->Vertex3;
          int y, dy;
          
          if (v1->y > v2->y)
            { TRIVERTEX *t = v1; v1 = v2; v2 = t; }
          if (v2->y > v3->y)
            {
              TRIVERTEX *t = v2; v2 = v3; v3 = t;
              if (v1->y > v2->y)
                { t = v1; v1 = v2; v2 = t; }
            }
          /* v1->y <= v2->y <= v3->y */

          dy = v3->y - v1->y;
          for (y = 0; y < dy; y++)
            {
              /* v1->y <= y < v3->y */
              TRIVERTEX *v = y < (v2->y - v1->y) ? v1 : v3;
              /* (v->y <= y < v2->y) || (v2->y <= y < v->y) */
              int dy2 = v2->y - v->y;
              int y2 = y + v1->y - v->y;

              int x1 = (v3->x     * y  + v1->x     * (dy  - y )) / dy;
              int x2 = (v2->x     * y2 + v-> x     * (dy2 - y2)) / dy2;
              int r1 = (v3->Red   * y  + v1->Red   * (dy  - y )) / dy;
              int r2 = (v2->Red   * y2 + v-> Red   * (dy2 - y2)) / dy2;
              int g1 = (v3->Green * y  + v1->Green * (dy  - y )) / dy;
              int g2 = (v2->Green * y2 + v-> Green * (dy2 - y2)) / dy2;
              int b1 = (v3->Blue  * y  + v1->Blue  * (dy  - y )) / dy;
              int b2 = (v2->Blue  * y2 + v-> Blue  * (dy2 - y2)) / dy2;
               
              int x;
              if (x1 < x2)
                {
                  int dx = x2 - x1;
                  for (x = 0; x < dx; x++)
                    SetPixel (hdc, x + x1, y + v1->y, RGB(
                      (r1 * (dx - x) + r2 * x) / dx >> 8,
                      (g1 * (dx - x) + g2 * x) / dx >> 8,
                      (b1 * (dx - x) + b2 * x) / dx >> 8));
                }
              else
                {
                  int dx = x1 - x2;
                  for (x = 0; x < dx; x++)
                    SetPixel (hdc, x + x2, y + v1->y, RGB(
                      (r2 * (dx - x) + r1 * x) / dx >> 8,
                      (g2 * (dx - x) + g1 * x) / dx >> 8,
                      (b2 * (dx - x) + b1 * x) / dx >> 8));
                }
            }
        }
      break;
    default:
      return FALSE;
  }

  return TRUE;
}

/******************************************************************************
 *           GdiTransparentBlt [GDI32.@]
 */
BOOL WINAPI TransparentBlt_NEW( HDC hdcDest, int xDest, int yDest, int widthDest, int heightDest,
                                HDC hdcSrc, int xSrc, int ySrc, int widthSrc, int heightSrc,
                                UINT crTransparent )
{
	BOOL ret = FALSE;
	HDC hdcWork;
	HBITMAP bmpWork;
	HGDIOBJ oldWork;
	HDC hdcMask = NULL;
	HBITMAP bmpMask = NULL;
	HBITMAP oldMask = NULL;
	COLORREF oldBackground;
	COLORREF oldForeground;
	int oldStretchMode;

	if ( !hdcDest || !hdcSrc )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if(widthDest < 0 || heightDest < 0 || widthSrc < 0 || heightSrc < 0) {
		return FALSE;
	}

	oldBackground = SetBkColor(hdcDest, RGB(255,255,255));
	oldForeground = SetTextColor(hdcDest, RGB(0,0,0));

	/* Stretch bitmap */
	oldStretchMode = GetStretchBltMode(hdcSrc);
	if(oldStretchMode == BLACKONWHITE || oldStretchMode == WHITEONBLACK)
		SetStretchBltMode(hdcSrc, COLORONCOLOR);
	hdcWork = CreateCompatibleDC(hdcDest);
	bmpWork = CreateCompatibleBitmap(hdcDest, widthDest, heightDest);
	oldWork = SelectObject(hdcWork, bmpWork);
	if(!StretchBlt(hdcWork, 0, 0, widthDest, heightDest, hdcSrc, xSrc, ySrc, widthSrc, heightSrc, SRCCOPY)) goto error;
	SetBkColor(hdcWork, crTransparent);

	/* Create mask */
	hdcMask = CreateCompatibleDC(hdcDest);
	bmpMask = CreateCompatibleBitmap(hdcMask, widthDest, heightDest);
	oldMask = SelectObject(hdcMask, bmpMask);
	if(!BitBlt(hdcMask, 0, 0, widthDest, heightDest, hdcWork, 0, 0, SRCCOPY)) goto error;

	/* Replace transparent color with black */
	SetBkColor(hdcWork, RGB(0,0,0));
	SetTextColor(hdcWork, RGB(255,255,255));
	if(!BitBlt(hdcWork, 0, 0, widthDest, heightDest, hdcMask, 0, 0, SRCAND)) goto error;

	/* Replace non-transparent area on destination with black */
	if(!BitBlt(hdcDest, xDest, yDest, widthDest, heightDest, hdcMask, 0, 0, SRCAND)) goto error;

	/* Draw the image */
	if(!BitBlt(hdcDest, xDest, yDest, widthDest, heightDest, hdcWork, 0, 0, SRCPAINT)) goto error;

	ret = TRUE;
error:
	SetStretchBltMode(hdcSrc, oldStretchMode);
	SetBkColor(hdcDest, oldBackground);
	SetTextColor(hdcDest, oldForeground);
	if(hdcWork) {
		SelectObject(hdcWork, oldWork);
		DeleteDC(hdcWork);
	}
	if(bmpWork) DeleteObject(bmpWork);
	if(hdcMask) {
		SelectObject(hdcMask, oldMask);
		DeleteDC(hdcMask);
	}
	if(bmpMask) DeleteObject(bmpMask);
	return ret;
}

void WINAPI vSetDdrawflag()
{
	return;
}
