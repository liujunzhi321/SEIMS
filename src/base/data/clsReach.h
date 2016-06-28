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

private:
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
	/// rate constant for biological oxidation of NH3 to NO2 in reach at 20 deg C
	float  bc1;
	/// rate constant for biological oxidation of NO2 to NO3 in reach at 20 deg C
	float  bc2;
	/// rate constant for biological oxidation of organic N to ammonia in reach at 20 deg C
	float  bc3;
	/// rate constant for biological oxidation of organic P to dissolved P in reach at 20 deg C
	float  bc4;
	/// local algal settling rate in reach at 20 deg C (m/day)
	float  rs1;	
	/// benthos source rate for dissolved phosphorus in reach at 20 deg C (mg disP-P)/((m**2)*day)
	float  rs2;
	/// benthos source rate for ammonia nitrogen in reach at 20 deg C (mg NH4-N)/((m**2)*day)
	float  rs3;	
	/// rate coefficient for organic nitrogen settling in reach at 20 deg C (1/day)
	float  rs4;	
	/// organic phosphorus settling rate in reach at 20 deg C (1/day)
	float  rs5;	
     /// CBOD deoxygenation rate coefficient in reach at 20 deg C (1/day)
	float  rk1;
     /// reaeration rate in accordance with Fickian diffusion in reach at 20 deg C (1/day)
	float  rk2; 
     /// rate of loss of CBOD due to settling in reach at 20 deg C (1/day)
	float  rk3;
     /// sediment oxygen demand rate in reach at 20 deg C (mg O2/ ((m**2)*day))
	float  rk4;

};

