//! Class to manage parameter information
#include "ParamInfo.h"
#include "util.h"
#include <iostream>
using namespace std;

//! define string constants used in the code
#define Use_Y "Y"
#define Use_RC "RC"
#define Use_AC "AC"
#define Use_NC "NC"

//! Constructor
ParamInfo::ParamInfo(void)
{
	Reset();
}

//! Destructor
ParamInfo::~ParamInfo(void)
{
}

//! Return the adjusted value for this parameter
float ParamInfo::GetAdjustedValue()
{
	float res = Value;

	if (StringMatch(Use, Use_Y))
	{
		//if (Change == Use_NC)
		//{
		//	don't change
		//}
		if (StringMatch(Change, Use_RC))
		{
			res = Value * Impact;
		}
		else if (StringMatch(Change, Use_AC))
		{
			res = Value + Impact;
		}
	}

	return res;
}
//! Adjust 1D array
void ParamInfo::Adjust1DArray(int n, float* data)
{
	float res = Value;

	if (StringMatch(Use, Use_Y))
	{
		if (StringMatch(Change, Use_RC) && !FloatEqual(Impact, 1.0))
		{
			for (int i = 0; i < n; i++)
			{
				data[i] *= Impact;
			}
		}
		else if (StringMatch(Change, Use_AC) && !FloatEqual(Impact, 0))
		{
			for (int i = 0; i < n; i++)
			{
				data[i] += Impact;
			}
		}
	}
}

//! Reset the contents of the object to default values
void ParamInfo::Reset(void)
{
	Change = "";
	Description = "";
	Dimension = DT_Unknown;
	Impact = 0.0;
	Max = 0.0;
	Min = 0.0;
	ModuleID = "";
	Name = "";
	Source = "";
	Units = "";
	Use = "";
	Value = 0.0;
	DependPara = NULL;
	IsConstant = false;
	ClimateType = "";
	IsOutput = false;
	OutputToOthers = false;
}
