#include "PlantManagementOperation.h"
#include <sstream>
#include <iomanip>
#include "utils.h"
#include "BMPText.h"

using namespace MainBMP;
using namespace PlantManagement;

/// Base class
PlantManagementOperation::PlantManagementOperation(int mgtOp, float husc, int year, int month, int day, float* parameters)
	:m_mgtOp(mgtOp),m_frHU(husc),m_year(year),m_month(month), m_day(day), m_parameters(parameters)
{
}
PlantManagementOperation::~PlantManagementOperation()
{
}

/// Plant
PlantOperation::PlantOperation(int mgtOp, float husc, int year, int month, int day,float* parameters):
PlantManagementOperation(mgtOp,husc,year,month,day,parameters)
{
	m_plantID = int(parameters[0]);
	m_curYrMat = int(parameters[2]);
	m_heatUnits = parameters[3];
	m_laiInit = parameters[4];
	m_bioInit = parameters[5];
	m_hiTarg = parameters[6];
	m_bioTarg = parameters[7];
	m_CNOP = parameters[8];
}
PlantOperation::~PlantOperation()
{
}
void PlantOperation::dump(ostream *fs)
{
	if(fs == NULL) return;
	*fs	<< "Plant Operation: " <<endl<< "HUSC: "<<m_frHU<<" rotationYear: "<<
		m_year<<" Month: "<<m_month<<" Day: "<<m_day<<
		" Plant ID: "<<m_plantID<< " CurrentYearToMaturity: " << m_curYrMat<<
		" Heat Units: "<<m_heatUnits<<" Initial LAI: "<<m_laiInit<<
		" Initial Biomass: "<<m_bioInit<<" Harvest Index: "<<m_hiTarg<<
		" Biomass target: "<<m_bioTarg<<" CNOP: "<<m_CNOP<<endl;
}
