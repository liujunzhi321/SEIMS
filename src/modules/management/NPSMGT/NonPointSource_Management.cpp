#include "api.h"
#include <iostream>
#include "NonPointSource_Management.h"
#include "MetadataInfo.h"
#include "ModelException.h"

using namespace std;

NPS_Management::NPS_Management(void) : m_nCells(-1), m_cellWidth(-1.f), m_timestep(-1.f), m_cellArea(-1.f),
	m_mgtFields(NULL),
	m_soilStorage(NULL), m_sol_no3(NULL), m_sol_nh3(NULL), m_sol_solp(NULL)
{
	m_arealSrcFactory.clear();
}

NPS_Management::~NPS_Management(void)
{
	if (!m_arealSrcFactory.empty())
	{
		for (map<int, BMPArealSrcFactory*>::iterator it = m_arealSrcFactory.begin(); it != m_arealSrcFactory.end();)
		{
			if(it->second != NULL)
				delete it->second;
			it = m_arealSrcFactory.erase(it);
		}
		m_arealSrcFactory.clear();
	}
}

bool NPS_Management::CheckInputSize(const char *key, int n)
{
	if (n <= 0)
		throw ModelException(MID_NPSMGT, "CheckInputSize",
		"Input data for " + string(key) + " is invalid. The size could not be less than zero.");
	if (this->m_nCells != n)
	{
		if (this->m_nCells <= 0) this->m_nCells = n;
		else
			throw ModelException(MID_NPSMGT, "CheckInputSize", "Input data for " + string(key) +
			" is invalid. All the input data should have same size.");
	}
	return true;
}

void NPS_Management::SetValue(const char *key, float data)
{
	string sk(key);
	if (StringMatch(sk, Tag_TimeStep)) m_timestep = data;
	else if (StringMatch(sk, Tag_CellWidth)) m_cellWidth = data;
	else
		throw ModelException(MID_NPSMGT, "SetValue", "Parameter " + sk + " does not exist.");
}

void NPS_Management::Set1DData(const char *key, int n, float *data)
{
	string sk(key);
	CheckInputSize(key, n);
	if (StringMatch(sk, VAR_MGT_FIELD))m_mgtFields = data;
	else
		throw ModelException(MID_NPSMGT, "Set1DData", "Parameter " + sk + " does not exist.");
}

void NPS_Management::Set2DData(const char *key, int n, int col, float **data)
{
	string sk(key);
	CheckInputSize(key,n);
	if (StringMatch(sk, VAR_SOL_ST)) m_soilStorage = data;
	else if (StringMatch(sk, VAR_SOL_NO3)) m_sol_no3 = data;
	else if (StringMatch(sk, VAR_SOL_NH3)) m_sol_nh3 = data;
	else if (StringMatch(sk, VAR_SOL_SOLP)) m_sol_solp = data;
	else
		throw ModelException(MID_NPSMGT, "Set2DData", "Parameter " + sk + " does not exist.");
}

void NPS_Management::SetScenario(Scenario *sce)
{
	if (sce != NULL){
		map <int, BMPFactory* > *tmpBMPFactories = sce->GetBMPFactories();
		for (map<int, BMPFactory *>::iterator it = tmpBMPFactories->begin(); it != tmpBMPFactories->end(); it++)
		{
			/// Key is uniqueBMPID, which is calculated by BMP_ID * 100000 + subScenario;
			if (it->first / 100000 == BMP_TYPE_AREALSOURCE)
			{
				m_arealSrcFactory[it->first] = (BMPArealSrcFactory*)it->second;
			}
		}
	}
	else
		throw ModelException(MID_MUSK_CH, "SetScenario", "The scenario can not to be NULL.");
}


int NPS_Management::Execute()
{
	if (m_cellArea < 0.f) m_cellArea = m_cellWidth * m_cellWidth;
	for (map<int, BMPArealSrcFactory*>::iterator it = m_arealSrcFactory.begin(); it != m_arealSrcFactory.end(); it++)
	{
		/// 1 Set valid cells index of areal source regions
		if(!it->second->GetLocationLoadStatus())
			it->second->SetArealSrcLocsMap(m_nCells, m_mgtFields);
		/// 2 Loop the management operations by sequence
		vector<int> tmpArealFieldIDs = it->second->GetArealSrcIDs();
		vector<int> tmpMgtSeqs = it->second->GetArealSrcMgtSeqs();
		//map<int, ArealSourceMgtParams *> tmpMgtParams = it->second->GetArealSrcMgtMap();
		//map<int, ArealSourceLocations *> tmpArealLocsMap = it->second->GetArealSrcLocsMap();
		for (vector<int>::iterator mgtSeqIter = tmpMgtSeqs.begin(); mgtSeqIter != tmpMgtSeqs.end(); mgtSeqIter++)
		{
			ArealSourceMgtParams *tmpMgtParams = it->second->GetArealSrcMgtMap().at(*mgtSeqIter);
			if(tmpMgtParams->GetStartDate() != 0 && tmpMgtParams->GetEndDate() != 0)
			{
				if (m_date < tmpMgtParams->GetStartDate() || m_date > tmpMgtParams->GetEndDate())
					continue;
			}
			for (vector<int>::iterator fIDIter = tmpArealFieldIDs.begin(); fIDIter != tmpArealFieldIDs.end(); fIDIter++)
			{
				float deltaWtrMM = 0.f, deltaNH3 = 0.f, deltaNO3 = 0.f, deltaOrgN = 0.f;
				float deltaMinP = 0.f, deltaOrgP = 0.f;
				ArealSourceLocations* tmpLoc = it->second->GetArealSrcLocsMap().at(*fIDIter);
				float tmpSize = tmpLoc->GetSize();
				int tmpNCells = tmpLoc->GetValidCells();
				if (tmpMgtParams->GetWaterVolume() > 0.f) /// m3/'size'/day ==> mm
					deltaWtrMM = tmpMgtParams->GetWaterVolume() * tmpSize * m_timestep / 86400.f / tmpNCells / m_cellArea * 1000.f;
				float cvt = tmpSize * m_timestep / 86400.f / tmpNCells / m_cellArea * 10000.f; /// kg/'size'/day ==> kg/ha
				if (tmpMgtParams->GetNH3() > 0.f)
					deltaNH3 = cvt * tmpMgtParams->GetNH3();
				if (tmpMgtParams->GetNO3() > 0.f)
					deltaNO3 = cvt * tmpMgtParams->GetNO3();
				if (tmpMgtParams->GetOrgN() > 0.f)
					deltaOrgN = cvt * tmpMgtParams->GetOrgN();
				if (tmpMgtParams->GetMinP() > 0.f)
					deltaMinP = cvt * tmpMgtParams->GetMinP();
				if (tmpMgtParams->GetOrgP() > 0.f)
					deltaOrgP = cvt * tmpMgtParams->GetOrgP();
				vector<int> tmpCellIdx = tmpLoc->GetCellsIndex();
				for (vector<int>::iterator idxIter = tmpCellIdx.begin(); idxIter != tmpCellIdx.end(); idxIter++)
				{
					/// add to the top first soil layer
					if (deltaWtrMM > 0.f && m_soilStorage != NULL)
						m_soilStorage[*idxIter][0] += deltaWtrMM;
					if (deltaNO3 > 0.f && m_sol_no3 != NULL)
						m_sol_no3[*idxIter][0] += deltaNO3;
					if (deltaNH3 > 0.f && m_sol_nh3 != NULL)
						m_sol_nh3[*idxIter][0] += deltaNH3;
					if (deltaMinP > 0.f && m_sol_solp != NULL)
						m_sol_solp[*idxIter][0] += deltaMinP;
				}
			}
		}
	}
    return true;
}

//void NPS_Management::Get1DData(const char *key, int *n, float **data)
//{
//}
//
//void NPS_Management::Get2DData(const char *key, int *n, int *col, float ***data)
//{
//}