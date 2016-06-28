#include "api.h"
#include <iostream>
#include "managementOperation_SWAT.h"
#include "MetadataInfo.h"
#include "ModelException.h"
using namespace std;
MGTOpt_SWAT::MGTOpt_SWAT(void):m_nCells(-1)
{
}


MGTOpt_SWAT::~MGTOpt_SWAT(void)
{
}

void MGTOpt_SWAT::Set1DData(const char* key, int n, float* data)
{
}

void MGTOpt_SWAT::Set2DData(const char* key, int n, int col, float** data)
{
}
bool MGTOpt_SWAT::GetOperationParameters(int i, int nOp)
{
	/// current management operation code
	/// and Julian day of current management operation'
	int curOpJDay;
	float curFrHU;
	float aphu; /// fraction of total heat units accumulated
	if(FloatEqual(m_igro[i], 0.))
		aphu = m_phuBase[i];
	else
		aphu = m_phuAcc[i];
	if(m_dormFlag[i] == 1)
		aphu = NODATA;
	return true;
}
void MGTOpt_SWAT::PlantOperation(int cellIdx)
{
	m_igro[cellIdx] = 1;

}
void MGTOpt_SWAT::ScheduledManagement(int cellIdx, int mgtOpCode)
{
	switch(mgtOpCode)
	{
		case 1:
			PlantOperation(cellIdx);
			break;
		case 2:
			IrrigationOperation(cellIdx);
			break;
		case 3:
			FertilizerOperation(cellIdx);
			break;
		case 4:
			PesticideOperation(cellIdx);
			break;
		case 5:
			HarvestKillOperation(cellIdx);
			break;
		case 6:
			TillageOperation(cellIdx);
			break;
		case 7:
			HarvestOnlyOperation(cellIdx);
			break;
		case 8:
			KillOperation(cellIdx);
			break;
		case 9:
			GrazingOperation(cellIdx);
			break;
		case 10:
			AutoIrrigationOperation(cellIdx);
			break;
		case 11:
			AutoFertilizerOperation(cellIdx);
			break;
		case 12:
			StreetSweepingOperation(cellIdx);
			break;
		case 13:
			ReleaseImpoundOperation(cellIdx);
			break;
		case 14:
			ContinuousFertilizerOperation(cellIdx);
			break;
		case 15:
			ContinuousPesticideOperation(cellIdx);
			break;
		case 16:
			BurningOperation(cellIdx);
			break;
	}
}
int MGTOpt_SWAT::Execute()
{
	m_jDay = JulianDay(m_date);
#pragma omp parallel for
	for (int i = 0; i < m_nCells; i++)
	{
		/// there might be several operation occurred on one day
		for (int j = m_curNOP[i]; j < m_maxNOP[i]; j++)
		{
			if (GetOperationParameters(i,j))
			{
				ScheduledManagement(i,m_curMgtOp[i]);
				if(m_curMgtOp[i] == 17)
					ScheduledManagement(i,m_curMgtOp[i]);
			}
		}
	}
	return true;
}

void MGTOpt_SWAT::Get1DData(const char* key, int* n, float** data)
{
}
void MGTOpt_SWAT::Get2DData(const char* key, int* n, int* col, float*** data)
{
}