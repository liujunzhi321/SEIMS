/*!
 * \file base_VariableData.h
 * \brief Base class used to store input and output variable datasets
 *
 *
 *
 * \author Junzhi Liu
 * \version 
 * \date June 2010
 *
 * 
 */

#pragma once

#include <string>
#include "MetadataInfoConst.h"
using namespace std;
/*!
 * \ingroup data
 * \class base_VariableData
 *
 * \brief Base class for variable data
 *
 *
 *
 */
class base_VariableData
{
protected:
	//! dimension type for the dataset, \a dimensionTypes
	dimensionTypes m_Dimension;

public:
	base_VariableData(void);
	base_VariableData(const base_VariableData &obj);
	virtual ~base_VariableData(void);

	virtual base_VariableData& operator= (const base_VariableData &obj);
	virtual dimensionTypes Dimension(void);
};

