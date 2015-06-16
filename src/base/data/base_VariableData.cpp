/** \defgroup data
 * \ingroup Base
 * \brief include classes related to data, eg. Measurement, HydroClimateData etc..
 *
 *
 *
 */

/*!
 * \file base_VariableData.cpp
 * \brief Base class of variable data
 *
 * Base class used to store various versions of the variable data
 *
 * \author Junzhi Liu
 * \version 
 * \date June 2010
 *
 * 
 */

#include "base_VariableData.h"

//! Constructor
base_VariableData::base_VariableData(void)
{
	m_Dimension = DT_Unknown;
}

//! Copy Constructor
base_VariableData::base_VariableData(const base_VariableData &obj)
{
	m_Dimension = obj.m_Dimension;
}

//! Destructor
base_VariableData::~base_VariableData(void)
{
}

//! Assignment operator overload
base_VariableData& base_VariableData::operator= (const base_VariableData &obj)
{
	m_Dimension = obj.m_Dimension;
	return *this;
}

//! Returns the dimension type for the object instance
dimensionTypes base_VariableData::Dimension(void)
{
	return DT_Unknown;
}

