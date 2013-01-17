/*
 *  KernelEx
 *  Copyright (C) 2009-2010, Xeno86
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

#pragma warning(disable:4786)

#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <exception>
#include <algorithm>
#include <cctype>
#include <sys/stat.h>
using namespace std;
#include "prep.h"

//////////////////////////////////////////////////////////////////////////

vector<export_entry_named> all_exports_named;
vector<export_entry_ordinal> all_exports_ordinal;
stringstream all_declarations;
time_t timestamp;

//////////////////////////////////////////////////////////////////////////

int count(const string& s, char c, size_t pos, size_t epos)
{
	int num = 0;
	if (epos == string::npos)
		epos = s.length();
	if (pos < 0 || pos >= epos)
		return 0;
	for (int i = pos ; i < epos ; i++)
		if (s[i] == c)
			num++;
	return num;
}

//////////////////////////////////////////////////////////////////////////

FileFinder::FileFinder()
{
}

FileFinder::FileFinder(const string& fn)
{
	search_for(fn);
}
	
FileFinder::~FileFinder()
{
}

void FileFinder::search_for(const string& fn)
{
	search_for(&fn, 1);
}

void FileFinder::search_for(const string fns[], int num)
{
	WIN32_FIND_DATA find_data;
	files.clear();
	pos = 0;
	for (int i = 0 ; i < num ; i++)
	{
		const string& fn = fns[i];
		HANDLE h = FindFirstFile(fn.c_str(), &find_data);
		if (h == INVALID_HANDLE_VALUE)
			continue;
		do 
			files.push_back(find_data.cFileName);
		while (FindNextFile(h, &find_data));
		FindClose(h);
	}
	stable_sort(files.begin(), files.end());
}
	
string FileFinder::get_next_file()
{
	if (files.empty() || pos >= files.size())
		return "";
	return files[pos++];
}

//////////////////////////////////////////////////////////////////////////

FileDeclParser::FileDeclParser(const char* file_name) throw(exception)
{
	this->file_name = file_name;
	ifstream f(file_name);
	if (!f)
		throw Exception("Failed to open file");

	stringstream buf;
	buf << f.rdbuf();

	buffer = buf.str();
}

void FileDeclParser::parse() throw(exception)
{
	size_t pos = 0;
	while ((pos = buffer.find("MAKE_EXPORT", pos)) != string::npos)
	{
		size_t lfpos = buffer.find('\n', pos);
		if (lfpos == string::npos)
			break;

		parse_make_export(buffer.substr(pos, lfpos - pos));
		pos += sizeof("MAKE_EXPORT");
	}

	pos = 0;
	while ((pos = buffer.find("FORWARD_TO_UNICOWS(", pos)) != string::npos)
	{
		size_t lfpos = buffer.find('\n', pos);
		if (lfpos == string::npos)
			break;

		if (!is_containing_line_commented(pos))
			parse_forward_to_unicows(buffer.substr(pos, lfpos - pos));
		pos = lfpos + 1;
	}

	int total_exports = exports_named.size() + exports_ordinal.size();
	if (total_exports == 0)
	{
		cout << "File doesn't contain exports" << endl;
		return;
	}

	prefilter();

	int dcount = 0;
	size_t lpos = 0; //last pos
	int nest_level = 0; // nest level

	while ((pos = buffer.find('(', lpos)) != string::npos)
	{
		nest_level += count(buffer, '{', lpos, pos)
					- count(buffer, '}', lpos, pos);

		lpos = pos + 1;
		
		if (nest_level == 0)
		{
			if (is_containing_line_commented(pos))
				continue;

			//at this level we have position of function opening brace '('
			//the hard part is finding beginning of function declaration

			size_t beg = buffer.find_last_of(";}", pos);
			if (beg == string::npos)
				beg = 0;

			while (is_containing_line_commented(beg) && beg != string::npos)
				beg = buffer.find_last_of(";}", beg - 1);

			if (beg == string::npos)
				beg = 0;
		
			if (beg)
				beg++;

			size_t end = buffer.find(')', pos);
			if (end == string::npos)
				continue;

			while (is_containing_line_commented(end) && end != string::npos)
				end = buffer.find(')', end + 1);

			if (end == string::npos)
				continue;

			end++;

			string decl = buffer.substr(beg, end - beg) + ';';
			filter_declaration(decl);

			int cnt = find_matching_export(decl);

			if (!cnt)
				continue;

			all_declarations << decl << endl;
			dcount += cnt;

			//cout << decl << endl;

			lpos = end + 1;
		}

	}	

	vector<export_entry_named>::iterator i1 = exports_named.begin();
	vector<export_entry_ordinal>::iterator i2 = exports_ordinal.begin();

	while (i1 != exports_named.end())
		all_exports_named.push_back(*i1++);
	while (i2 != exports_ordinal.end())
		all_exports_ordinal.push_back(*i2++);

	if (dcount < total_exports)
		cout << "!Unresolved exports: " << total_exports - dcount << endl;
}

size_t FileDeclParser::find_line_start(size_t pos)
{
	size_t line_start = buffer.rfind('\n', pos);
	if (line_start == string::npos)
		line_start = 0;
	else
		line_start++;
	return line_start;
}

bool FileDeclParser::is_containing_line_commented(size_t pos)
{
	size_t line_start = find_line_start(pos);
	string p = buffer.substr(line_start, pos - line_start);
	size_t pp;
	if ((pp = p.find("//")) != string::npos || (pp = p.find('#')) != string::npos)
	{
		for (int i = 0 ; i < pp ; i++)
			// if there is something before '//' then the line isn't commented
			if (p[i] != ' ' && p[i] != '\t')
				return false;
		return true;
	}
	return false;
}

void FileDeclParser::parse_make_export(const string& line) throw(exception)
{
	size_t pos = line.find("MAKE_EXPORT");
	if (pos == string::npos)
		throw Exception("MAKE_EXPORT tag not found");
	pos += sizeof("MAKE_EXPORT");
	pos = line.find_first_not_of(" \t", pos);
	if (pos == string::npos)
		throw Exception("Invalid MAKE_EXPORT tag");
	size_t pos2 = line.find('=', pos);
	if (pos2 == string::npos)
		throw Exception("Invalid MAKE_EXPORT tag");
	string source_name = line.substr(pos, pos2 - pos);
	pos = pos2 + 1;
	pos2 = line.find_first_of(" \t", pos);
	if (line.compare(pos, sizeof("ordinal") - 1, "ordinal"))
	{
		export_entry_named ee;
		ee.source_name = source_name;
		ee.export_name = line.substr(pos, pos2 - pos);		
		exports_named.push_back(ee);
	}
	else
	{
		export_entry_ordinal ee;
		ee.source_name = source_name;
		ee.ordinal = strtol(line.substr(pos + sizeof("ordinal") - 1).c_str(), NULL, 0);
		exports_ordinal.push_back(ee);
	}
}

void FileDeclParser::parse_forward_to_unicows(const string& line) throw(exception)
{
	size_t pos = line.find("FORWARD_TO_UNICOWS(");
	if (pos == string::npos)
		throw Exception("FORWARD_TO_UNICOWS(");
	pos += sizeof("FORWARD_TO_UNICOWS(") - 1;
	size_t pos2 = line.find(')', pos);
	if (pos2 == string::npos)
		throw Exception("Invalid FORWARD_TO_UNICOWS tag");
	string name = line.substr(pos, pos2 - pos);
	export_entry_named ee;
	ee.source_name = name + "_fwd";
	ee.export_name = name;
	all_exports_named.push_back(ee);
	all_declarations << "FWDPROC " + name + "_fwd;" << endl;
}

int FileDeclParser::find_matching_export(const string& s)
{
	int count = 0;
	size_t pos;

	//try named exports first
	vector<export_entry_named>::iterator itn = exports_named.begin();
	while (itn != exports_named.end())
	{
		pos = s.find(itn->source_name);
		if (pos != string::npos && is_full_decl(s, pos, itn->source_name.length()))
			count++;
		itn++;
	}
	//then try ordinal exports
	vector<export_entry_ordinal>::iterator ito = exports_ordinal.begin();
	while (ito != exports_ordinal.end())
	{
		pos = s.find(ito->source_name);
		if (pos != string::npos && is_full_decl(s, pos, ito->source_name.length()))
			count++;
		ito++;
	}
	return count;
}

bool FileDeclParser::is_full_decl(const string& s, int beg, int len)
{
	if (beg > 0 && (isalnum(s[beg-1]) || s[beg-1] == '_'))
		return false;
	if (beg + len < s.length() && (isalnum(s[beg+len]) || s[beg+len] == '_'))
		return false;
	return true;
}

void FileDeclParser::prefilter() throw(exception)
{
	//erase C-style comments
	size_t pos;
	while ((pos = buffer.find("/*")) != string::npos)
	{
		size_t pos2 = buffer.find("*/", pos + 1);
		if (pos2 == string::npos)
			throw Exception("No matching C-style comment closing");
		buffer.erase(pos, pos2 - pos + sizeof("*/") - 1);
	}
}

void FileDeclParser::filter_declaration(string& s)
{
	int i = 0;

	//erase precompiler directives
	size_t pos;
	while ((pos = s.find('#')) != string::npos)
	{
		size_t p2 = s.find('\n', pos + 1);
		if (p2 != string::npos)
			p2 -= pos;
		s.erase(pos, p2);
	}

	//erase C++ style comments
	while ((pos = s.find("//")) != string::npos)
	{
		size_t p2 = s.find('\n', pos + 1);
		if (p2 != string::npos)
			p2 -= pos;
		s.erase(pos, p2);
	}

	//erase "__declspec(naked)"
	while ((pos = s.find("__declspec(naked)")) != string::npos)
		s.erase(pos, sizeof("__declspec(naked)"));

	//replace tabs and newlines with spaces
	for (i = 0 ; i < s.length() ; i++)
		if (s[i] == '\t' || s[i] == '\n')
			s[i] = ' ';

	//erase leading spaces
	while (s[0] == ' ')
		s.erase(0, 1);


	//erase spaces after bracket
	pos = s.find('(');
	while (s[pos + 1] == ' ')
		s.erase(pos + 1, 1);

	//erase spaces before bracket
	pos = s.rfind(')');
	while (s[pos - 1] == ' ')
	{
		s.erase(pos - 1, 1);
		pos--;
	}

	//erase multiple spaces
	i = 0;
	while (i < s.length())
	{
		if (s[i] == ' ')
			while (i + 1 < s.length() && s[i + 1] == ' ')
				s.erase(i + 1, 1);
		i++;
	}
}

//////////////////////////////////////////////////////////////////////////

StubsParser::StubsParser(const char* file_name) throw(exception) : FileDeclParser(file_name)
{
}

void StubsParser::parse() throw(exception)
{
	prefilter();
	
	size_t pos = 0;
	size_t lpos = 0;

	while ((pos = buffer.find("UNIMPL_FUNC", lpos)) != string::npos)
	{
		size_t beg = buffer.find('(', pos);
		if (beg == string::npos)
			continue;
		beg++;

		while (isspace(buffer[beg]))
			beg++;

		size_t end = buffer.find(',', beg + 1);
		if (end == string::npos)
			continue;
		end--;

		while (isspace(buffer[end]))
			end--;

		string name = buffer.substr(beg, end - beg + 1);
		string decl = "STUB " + name + "_stub" + ';';
		all_declarations << decl << endl;
		export_entry_named ee;
		ee.export_name = name;
		ee.source_name = name + "_stub";
		all_exports_named.push_back(ee);

		lpos = end + 1;
	}
}

//////////////////////////////////////////////////////////////////////////

void write_named_exports(ostream& o)
{
	vector<export_entry_named>::iterator it = all_exports_named.begin();
	while (it != all_exports_named.end()) 
	{
		o << "\tDECL_API(\"" << it->export_name << "\", " 
				<< it->source_name << ")," << endl;
		it++;
	}
}

void write_ordinal_exports(ostream& o)
{
	vector<export_entry_ordinal>::iterator it = all_exports_ordinal.begin();
	while (it != all_exports_ordinal.end()) 
	{
		o << "\tDECL_API(" << it->ordinal << ", " 
				<< it->source_name << ")," << endl;
		it++;
	}
}

void dump_all()
{
	cout << "*** function declarations:" << endl;
	cout << all_declarations.rdbuf();
	cout << "*** named exports:" << endl;
	write_named_exports(cout);
	cout << "*** ordinal exports:" << endl;
	write_ordinal_exports(cout);
}

void replace(const string& out, const string& in)
{
	ifstream fin;
	ofstream fout;

	fin.open(in.c_str(), ios::in);
	if (!fin)
		throw Exception("Failed to open file");

	fout.open(out.c_str(), ios::out | ios::trunc);
	if (!fout)
		throw Exception("Failed to open file");
	fout << fin.rdbuf();

	fout.close();
	fin.close();

	remove(in.c_str());
}

bool is_uptodate_dir(const string& path)
{
	string file;
	FileFinder ff;
	struct _stat st;

	string patterns[] = { path + "*.c", path + "*.cpp" };
	ff.search_for(patterns, sizeof(patterns)/sizeof(patterns[0]));
	while (!(file = ff.get_next_file()).empty())
	{
		_stat((path + file).c_str(), &st);
		if (st.st_mtime > timestamp)
			return false;
	}

	return true;
}

void work()
{
	ifstream dirlist("dirlist");
	if (!dirlist)
		throw Exception("Couldn't open dirlist");

	{
		struct _stat st;
		if (_stat(".timestamp", &st) < 0)
			timestamp = 0;
		else
			timestamp = st.st_mtime;
	}

	while (!dirlist.eof())
	{
		fstream out_file;
		stringstream buf;
		string path;
		string file;
		FileFinder ff;
		string line;

		all_exports_named.clear();
		all_exports_ordinal.clear();
		all_declarations.clear();
		all_declarations.str("");

		getline(dirlist, path);

		//skip empty lines
		if (path.empty())
			continue;

		path += '\\';

		cout << "Processing directory: '" << path << '\'' << endl;
		if (is_uptodate_dir(path))
		{
			cout << "Directory is up to date" << endl;
			continue;
		}
		
		string patterns[] = { path + "*.c", path + "*.cpp" };
		ff.search_for(patterns, sizeof(patterns)/sizeof(patterns[0]));

		//read declarations
		while (!(file = ff.get_next_file()).empty())
		{
			FileDeclParser* parser;

			file = path + file;

			cout << "Parsing file: '" << file << '\'' << endl;

			if (file.find("_stubs.") != string::npos)
				parser = new StubsParser(file.c_str());
			else
				parser = new FileDeclParser(file.c_str());

			parser->parse();
			delete parser;
		}

		stable_sort(all_exports_named.begin(), all_exports_named.end());
		stable_sort(all_exports_ordinal.begin(), all_exports_ordinal.end());

		//write output defs
		ff.search_for(path + "_*_apilist.c");
		file = ff.get_next_file();
		if (file.empty())
			throw Exception("Couldn't find output def file");

		file = path + file;

		cout << "Definitions output: '" << file << '\'' << endl;

		out_file.open(file.c_str(), fstream::in);
		if (!out_file.is_open())
			throw Exception("Failed to open output def file for read");

		buf << out_file.rdbuf();

		out_file.close();

		out_file.open((file + ".tmp").c_str(), ios::out | ios::trunc);
		if (!out_file.is_open())
			throw Exception("Failed to open output def file for write");

		while (true)
		{
			getline(buf, line);
			if (0 == line.compare("/*** AUTOGENERATED APILIST NAMED EXPORTS BEGIN ***/"))
			{
				out_file << line << endl;
				while (true)
				{
					getline(buf,line);
					if (0 == line.compare("/*** AUTOGENERATED APILIST NAMED EXPORTS END ***/"))
						break;
				}

				write_named_exports(out_file);
			}
			else if (0 == line.compare("/*** AUTOGENERATED APILIST ORDINAL EXPORTS BEGIN ***/"))
			{
				out_file << line << endl;
				while (true)
				{
					getline(buf,line);
					if (0 == line.compare("/*** AUTOGENERATED APILIST ORDINAL EXPORTS END ***/"))
						break;
				}

				write_ordinal_exports(out_file);
			}
			out_file << line;

			if (!buf.eof())
				out_file << endl;
			else
				break;
		}

		out_file.close();
		replace(file, file + ".tmp");

		//write output decls
		ff.search_for(path + "_*_apilist.h");

		file = ff.get_next_file();
		if (file.empty())
			throw Exception("Couldn't find output decl file");

		file = path + file;

		cout << "Declarations output: '" << file << '\'' << endl;

		out_file.open(file.c_str(), fstream::in);
		if (!out_file.is_open())
			throw Exception("Failed to open output decl file for read");

		buf.clear();
		buf.str("");
		buf << out_file.rdbuf();

		out_file.close();

		out_file.open((file + ".tmp").c_str(), ios::out | ios::trunc);
		if (!out_file.is_open())
			throw Exception("Failed to open output decl file for write");

		while (true) 
		{
			getline(buf, line);
			if (0 == line.compare("/*** AUTOGENERATED APILIST DECLARATIONS BEGIN ***/"))
			{
				out_file << line << endl;
				while (true)
				{
					getline(buf,line);
					if (0 == line.compare("/*** AUTOGENERATED APILIST DECLARATIONS END ***/"))
						break;
				}

				all_declarations.peek();
				if (all_declarations.good())
					out_file << all_declarations.rdbuf();
			}
			out_file << line;

			if (!buf.eof())
				out_file << endl;
			else
				break;
		}

		out_file.close();
		replace(file, file + ".tmp");

		ofstream timestamp(".timestamp", ios::out | ios::trunc);
	}

}

int main(int argc, char* argv[])
{
	cout << "KernelEx source preparser by Xeno86" << endl;

	if (argc > 2)
	{
		cout << "Invalid parameters!" << endl;
		return 1;
	}
	
	if (argc == 2)
	{
		cout << "Processing project at: " << argv[1] << endl;
		if (!SetCurrentDirectory(argv[1]))
		{
			cout << "Failed to change directory" << endl;
			return 1;
		}
	}
	
	try
	{
		work();
	}
	catch (const exception& e)
	{
		cout << "Exception: " << e.what() << endl;
		return 1;
	}

	return 0;
}
