/*
 *  KernelEx
 *  Copyright (C) 2010, Xeno86
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

#include "common.h"
#include <mmsystem.h>

/* MAKE_EXPORT waveInGetDevCapsW_new=waveInGetDevCapsW */
MMRESULT WINAPI waveInGetDevCapsW_new(
	UINT uDeviceID,
	LPWAVEINCAPSW pwicW,
	UINT cbwic
)
{
	MMRESULT ret;
	WAVEINCAPSA wicA;
	WAVEINCAPSW wicW;

	if (pwicW == NULL)
		return MMSYSERR_INVALPARAM;

	ret = waveInGetDevCapsA(uDeviceID, &wicA, sizeof(wicA));

	if (ret == MMSYSERR_NOERROR)
	{
		wicW.wMid = wicA.wMid;
		wicW.wPid = wicA.wPid;
		wicW.vDriverVersion = wicA.vDriverVersion;
		MultiByteToWideChar(CP_ACP, 0, wicA.szPname, -1, wicW.szPname, sizeof(wicW.szPname));
		wicW.dwFormats = wicA.dwFormats;
		wicW.wChannels = wicA.wChannels;
		wicW.wReserved1 = wicA.wReserved1;

		memcpy(pwicW, &wicW, min(sizeof(wicW), cbwic));
	}

	return ret;
}

/* MAKE_EXPORT waveOutGetDevCapsW_new=waveOutGetDevCapsW */
MMRESULT WINAPI waveOutGetDevCapsW_new(
	UINT uDeviceID,
	LPWAVEOUTCAPSW pwocW,
	UINT cbwoc
)
{
	MMRESULT ret;
	WAVEOUTCAPSA wocA;
	WAVEOUTCAPSW wocW;

	if (pwocW == NULL)
		return MMSYSERR_INVALPARAM;
	
	ret = waveOutGetDevCapsA(uDeviceID, &wocA, sizeof(wocA));

	if (ret == MMSYSERR_NOERROR)
	{
		wocW.wMid = wocA.wMid;
		wocW.wPid = wocA.wPid;
		wocW.vDriverVersion = wocA.vDriverVersion;
		MultiByteToWideChar(CP_ACP, 0, wocA.szPname, -1, wocW.szPname, sizeof(wocW.szPname));
		wocW.dwFormats = wocA.dwFormats;
		wocW.wChannels = wocA.wChannels;
		wocW.wReserved1 = wocA.wReserved1;
		wocW.dwSupport = wocA.dwSupport;
	
		memcpy(pwocW, &wocW, min(sizeof(wocW), cbwoc));
	}

	return ret;
}
