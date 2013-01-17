/*
 *  KernelEx
 *  Copyright (C) 2009, Xeno86
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

#ifndef __DEBUG_WINDOW_H
#define __DEBUG_WINDOW_H

#include "sstring.hpp"
#pragma warning(disable:4530) //we don't do exception handling
#include <list>
#pragma warning(default:4530)

using namespace std;

class DebugWindow
{
public:
	DebugWindow();
	~DebugWindow();
	void msgloop();
	void append(const char* str);

private:
	HWND hwnd;
	HWND hList;
	HMENU menu;
	list<sstring> includes;
	list<sstring> excludes;

	static BOOL CALLBACK DebugDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK FilterDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void InitDialog(HWND hwnd);
	void HandleMenu(HWND hwnd);
	void DeleteSelItems();
	void ListView_Append(char* msg);
	void WriteToFile();
};

#endif
