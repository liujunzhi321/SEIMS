//! Base class used to store input and output variable datasets
#pragma once

#include <string>
#include "MetadataInfoConst.h"

using namespace std;

class base_VariableData
{
protected:
	// dimension type for the dataset
	dimensionTypes m_Dimension;

public:
	base_VariableData(void);
	base_VariableData(const base_VariableData &obj);
	virtual ~base_VariableData(void);

	virtual base_VariableData& operator= (const base_VariableData &obj);
	virtual dimensionTypes Dimension(void);
};

