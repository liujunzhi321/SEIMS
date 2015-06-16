/*!
 * \file StringList.h
 * \brief Utility class to store a list of strings
 *
 * 
 *
 * 
 */

#pragma once

#include <string>
#include <vector>

using namespace std;
/*!
 * \ingroup Util
 * \class StringList
 *
 * \brief Common operation of string vector
 *
 *
 *
 */
class StringList
{
private:
	vector<string> m_List;

public:
	StringList(void);
	~StringList(void);

	StringList& operator=(const StringList &obj);

	void Add(string item);
	void Insert(int index, string item);
	void Append(string item);
	bool Contains(string item);
	int Count(void);
	void Clear(void);
	
	string At(int postion);
	void Resize(int newsize, string value);
};

