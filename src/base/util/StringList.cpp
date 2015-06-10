/*!
 * \file StringList.cpp
 * \brief methods for the StringList utility class
 *
 * Implementation of the methods for the StringList utility class
 *
 *
 * 
 */
#include "StringList.h"
#include "util.h"

//! Constructor
StringList::StringList(void)
{
}

//! Destructor
StringList::~StringList(void)
{
	m_List.clear();
}

//! Add an item to the list
void StringList::Add(string item)
{
	m_List.push_back(item);
}

//! Insert an item into the list at the given position
void StringList::Insert(int index, string item)
{
	m_List[index] = item;
}

//! Append an item to the end of the list
void StringList::Append(string item)
{
	m_List.insert(m_List.end(), item);
}

//! Return a flag indicating if the given string is in the list
bool StringList::Contains(string item)
{
	bool bStatus = false;

	if (m_List.size() > 0)
	{
		for (size_t i=0; i<m_List.size(); i++)
		{
			if (StringMatch(m_List[i], item))
			{
				bStatus = true;
			}
		}
	}

	return bStatus;
}

//! Return the number of items in the list
int StringList::Count(void)
{
	return m_List.size();
}

//! Clear the list
void StringList::Clear(void)
{
	m_List.clear();
}

//! Get the item in the list at the given position
string StringList::At(int position)
{
	string res = "";

	if (position >= 0 && position < (int)m_List.size())
	{
		res = m_List[position];
	}

	return res;
}

//! Assignment operator overload
StringList& StringList::operator=(const StringList &obj)
{
	m_List.resize(obj.m_List.size());
	for (size_t itm=0; itm<obj.m_List.size(); itm++)
	{
		m_List[itm] = obj.m_List[itm];
	}
	return *this;
}

//! Resize the list to the given size with the given default values
void StringList::Resize(int newsize, string value)
{
	m_List.resize(newsize, value);
}