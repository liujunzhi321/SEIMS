/*!
 * \file ParamInfo.cpp
 * \brief Class to store parameter item information
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date June 2010
 *
 * 
 */
#include "ParamInfo.h"
#include "util.h"
#include <iostream>
using namespace std;




ParamInfo::ParamInfo(void)
{
	Reset();
}

ParamInfo::~ParamInfo(void)
{
}

float ParamInfo::GetAdjustedValue()
{
	float res = Value;

	if (StringMatch(Use, PARAM_USE_Y))
	{
		//if (Change == PARAM_CHANGE_NC)
		//{
		//	don't change
		//}
		if (StringMatch(Change, PARAM_CHANGE_RC))
		{
			res = Value * Impact;
		}
		else if (StringMatch(Change, PARAM_CHANGE_AC))
		{
			res = Value + Impact;
		}
	}

	return res;
}

void ParamInfo::Adjust1DArray(int n, float* data)
{
	float res = Value;

	if (StringMatch(Use, PARAM_USE_Y))
	{
		if (StringMatch(Change, PARAM_CHANGE_RC) && !FloatEqual(Impact, 1.0))
		{
			for (int i = 0; i < n; i++)
			{
				data[i] *= Impact;
			}
		}
		else if (StringMatch(Change, PARAM_CHANGE_AC) && !FloatEqual(Impact, 0))
		{
			for (int i = 0; i < n; i++)
			{
				data[i] += Impact;
			}
		}
	}
}

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
