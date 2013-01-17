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

#ifndef __PREP_H
#define __PREP_H

//////////////////////////////////////////////////////////////////////////

int count(const string& s, char c, size_t pos = 0, size_t epos = string::npos);

//////////////////////////////////////////////////////////////////////////

class Exception : public exception
{
private:
	string reason;
	
public:
	Exception(const char* what) : reason(what) {}
	~Exception() throw() {}
	const char* what() const throw() { return reason.c_str(); }
};

//////////////////////////////////////////////////////////////////////////

class FileFinder
{
public:
	FileFinder();
	FileFinder(const string& fn);
	~FileFinder();
	void search_for(const string& fn);
	void search_for(const string fns[], int num);
	string get_next_file();

private:
	int pos;
	vector<string> files;
};

//////////////////////////////////////////////////////////////////////////

struct export_entry_named
{
	string source_name;
	string export_name;

	bool operator<(const export_entry_named& e) const
	{
		return this->export_name.compare(e.export_name) < 0;
	}
};

struct export_entry_ordinal
{
	string source_name;
	unsigned short ordinal;

	export_entry_ordinal() : ordinal(0) {}
	bool operator<(const export_entry_ordinal& e) const
	{
		return this->ordinal < e.ordinal;
	}
};

//////////////////////////////////////////////////////////////////////////

class FileDeclParser
{
public:
	FileDeclParser(const char* file_name) throw(exception);
	virtual void parse() throw(exception);

protected:
	string buffer;
	string file_name;
	vector<export_entry_named> exports_named;
	vector<export_entry_ordinal> exports_ordinal;

	size_t find_line_start(size_t pos);
	bool is_containing_line_commented(size_t pos);
	void prefilter() throw(exception);
	void parse_make_export(const string& line) throw(exception);
	void parse_forward_to_unicows(const string& line) throw(exception);
	int find_matching_export(const string& s);
	void filter_declaration(string& s);

private:
	bool is_full_decl(const string& s, int beg, int len);
};

//////////////////////////////////////////////////////////////////////////

class StubsParser : public FileDeclParser
{
public:
	StubsParser(const char* file_name) throw(exception);
	void parse() throw(exception);
};

//////////////////////////////////////////////////////////////////////////

#endif
