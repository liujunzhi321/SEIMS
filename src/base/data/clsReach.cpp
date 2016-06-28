/*!
 * \file clsReach.cpp
 * \brief Class to store reach related parameters from REACHES table
 *
 * \author LiangJun Zhu
 * \version 1.0
 * \date May. 2016
 *
 */
#include "clsReach.h"
#include "util.h"
#include <iostream>
using namespace std;




clsReach::clsReach(void)
{
	Reset();
}

clsReach::~clsReach(void)
{
}

void clsReach::Reset(void)
{
	Area						= NODATA_VALUE;
	Depth					= NODATA_VALUE;
	DownStream		= -1;
	DownUpOrder		= -1;
	Group					= -1;
	GroupDivided		= -1;
	Length					= NODATA_VALUE;
	Manning				= NODATA_VALUE;
	NumCells			= -1;
	Slope					= NODATA_VALUE;
	Subbasin				= -1;
	UpDownOrder		= -1;
	V0						= NODATA_VALUE;
	Width					=NODATA_VALUE;
	bc1						= -1;
	bc2						= -1;
	bc3						= -1;
	bc4						= -1;
	rk1						= -1;
	rk2						= -1;
	rk3						= -1;
	rk4						= -1;
	rs1						= -1;
	rs2						= -1;
	rs3						= -1;
	rs4						= -1;
	rs5 						= -1;
}
