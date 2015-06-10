/*!
 * \file ArealParameter.cpp
 * \brief 
 *
 * 
 *
 * \author [your name]
 * \version 
 * \date June 2015
 *
 * 
 */
#include "ArealParameter.h"
#include <stdlib.h>

using namespace MainBMP;
using namespace BMPParameter;

ArealParameter::ArealParameter(int id,string name,string descri)
{
	m_description = descri;
	m_id = id;
	m_name = name;
}

ArealParameter::ArealParameter(vector<string> *oneRow)
{
	if(oneRow == NULL)			throw ModelException("ArealParameter","ArealParameter","oneRow can not be NULL!");
	if(int(oneRow->size()) <3)	throw ModelException("ArealParameter","ArealParameter","oneRow should have 3 parameters at least!");

	m_id = atoi(oneRow->at(0).c_str());
	m_name = oneRow->at(1);
	m_description = oneRow->at(2);
}


ArealParameter::~ArealParameter(void)
{
}

void ArealParameter::Dump(ostream* fs)
{
	if(fs == NULL) return;

	*fs	<< "parameter: id = " << m_id
		<< " name = " << m_name
		<< " description = " << m_description << endl;
}
