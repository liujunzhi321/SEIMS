/*!
 * \file clsReach.h
 * \brief Class to store reach related parameters from REACHES table
 *
 * \author LiangJun Zhu
 * \version 1.0
 * \date May. 2016
 *
 */
#pragma once

#include <string>
#include "MetadataInfoConst.h"

using namespace std;
/*!
* \ingroup data
 * \class clsReach
 * \brief Class to store reach related parameters from REACHES table
 */
class clsReach
{
public:
	//! Constructor
	clsReach(void);
	//! Destructor
	~clsReach(void);
	//! Reset the contents of the object to default values
	void Reset(void);
	
	//! Area
	float		Area;
	//! Depth
	float		Depth;
	//! Downstream reach index
	int			DownStream;
	//! DOWN_UP stream order
	int			DownUpOrder;
	//! Group Index
	int			Group;
	//! Group divided
	int			GroupDivided;
	//! Length
	float		Length;
	//! Manning coefficient
	float		Manning;
	//! Cell numbers
	int			NumCells;
	//! Slope gradient
	float		Slope;
	//! Subbasin ID
	int			Subbasin;
	//! UP_DOWN stream order
	int			UpDownOrder;
	//! V0
	float		V0;
	//! Width
	float		Width;
};

