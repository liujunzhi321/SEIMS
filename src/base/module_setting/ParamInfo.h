//! Class to store parameter information
#pragma once

#include <string>
#include "MetadataInfoConst.h"

using namespace std;

//! Class to store parameter information from the parameter database
class ParamInfo
{
public:
	ParamInfo(void);
	~ParamInfo(void);

	void Reset(void);

	float GetAdjustedValue();
	void Adjust1DArray(int n, float* data);
	string Name;
	string Units;
	string Description;
	string ModuleID;
	dimensionTypes Dimension;
	string Source;
	float Value;
	float Impact;
	string Change;
	float Max;
	float Min;
	string Use;

	ParamInfo* DependPara;
	string ClimateType;
	bool IsConstant;
	bool IsOutput;
	bool OutputToOthers;
	string BasicName;
};

