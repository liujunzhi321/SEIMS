#include "SedBonNutrient_CH.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>
#include <cmath>
#include <iostream>
#include <fstream>

#include <omp.h>
//#define NODATA_VALUE -99
using namespace std;

SedBonNutrient_CH::SedBonNutrient_CH(void) : m_nCells(-1), m_dt(-1.0f), m_CellWidth(-1.0f), m_flowInIndex(NULL),
                                             m_flowOutIndex(NULL),
                                             m_reachId(NULL), m_streamOrder(NULL), m_sourceCellIds(NULL),
                                             m_streamLink(NULL), m_Slope(NULL), m_SedBonP(NULL),
                                             m_ChannelWH(NULL), m_ChQkin(NULL), m_SedBonPToCh(NULL),
                                             m_SedBonAmmoniumToCh(NULL), m_chWidth(NULL), m_SedBonAmmonium(NULL),
                                             m_ChVol(NULL), m_SedBonP_KG(NULL), m_SedBonAmmonium_KG(NULL)
{
    omp_set_num_threads(1);
}

SedBonNutrient_CH::~SedBonNutrient_CH(void)
{
    if (m_SedBonP != NULL)
    {
        for (int i = 0; i < m_chNumber; ++i)
        {
            delete[] m_SedBonP[i];
        }
        delete[] m_SedBonP;
    }

    if (m_SedBonAmmonium != NULL)
    {
        for (int i = 0; i < m_chNumber; ++i)
        {
            delete[] m_SedBonAmmonium[i];
        }
        delete[] m_SedBonAmmonium;
    }

    if (m_SedBonP_KG != NULL)
    {
        for (int i = 0; i < m_chNumber; ++i)
        {
            delete[] m_SedBonP_KG[i];
        }
        delete[] m_SedBonP_KG;
    }

    if (m_SedBonAmmonium_KG != NULL)
    {
        for (int i = 0; i < m_chNumber; ++i)
        {
            delete[] m_SedBonAmmonium_KG[i];
        }
        delete[] m_SedBonAmmonium_KG;
    }

    if (this->m_ChVol != NULL)
    {
        for (int i = 0; i < m_chNumber; ++i)
            delete[] m_ChVol[i];
        delete[] this->m_ChVol;
    }
}

bool SedBonNutrient_CH::CheckInputData(void)
{
    if (this->m_date <= 0)
    {
        throw ModelException("SedBonNutrient_CH", "CheckInputData", "You have not set the Date variable.");
        return false;
    }

    if (this->m_nCells <= 0)
    {
        throw ModelException("SedBonNutrient_CH", "CheckInputData",
                             "The cell number of the input can not be less than zero.");
        return false;
    }

    if (this->m_dt <= 0)
    {
        throw ModelException("SedBonNutrient_CH", "CheckInputData", "You have not set the TimeStep variable.");
        return false;
    }

    if (this->m_CellWidth <= 0)
    {
        throw ModelException("SedBonNutrient_CH", "CheckInputData", "You have not set the CellWidth variable.");
        return false;
    }
    if (this->m_Slope <= 0)
    {
        throw ModelException("SedBonNutrient_CH", "CheckInputData", "You have not set the slope.");
        return false;
    }
    if (m_flowInIndex == NULL)
        throw ModelException("SedBonNutrient_CH", "CheckInputData", "The parameter: flow in index has not been set.");

    if (this->m_chWidth == NULL)
    {
        throw ModelException("SedBonNutrient_CH", "CheckInputData", "Channel width can not be NULL.");
        return false;
    }
    if (m_ChannelWH == NULL)
        throw ModelException("SedBonNutrient_CH", "CheckInputData",
                             "The parameter: Channel water hight has not been set.");
    if (m_ChQkin == NULL)
        throw ModelException("SedBonNutrient_CH", "CheckInputData",
                             "The parameter: Channel water flow has not been set.");
    if (m_SedBonPToCh == NULL)
        throw ModelException("SedBonNutrient_CH", "CheckInputData", "The parameter: m_SedBonPToCh has not been set.");
    if (m_SedBonAmmoniumToCh == NULL)
        throw ModelException("SedBonNutrient_CH", "CheckInputData",
                             "The parameter: m_SedBonAmmoniumToCh has not been set.");

    return true;
}

bool SedBonNutrient_CH::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
    {
        //this->StatusMsg("Input data for "+string(key) +" is invalid. The size could not be less than zero.");
        return false;
    }
    if (this->m_nCells != n)
    {
        if (this->m_nCells <= 0) this->m_nCells = n;
        else
        {
            //this->StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
            ostringstream oss;
            oss << "Input data for " + string(key) << " is invalid with size: " << n << ". The origin size is " <<
            m_nCells << ".\n";
            throw ModelException("SedBonNutrient_CH", "CheckInputSize", oss.str());
        }
    }

    return true;
}

void SedBonNutrient_CH::SetValue(const char *key, float data)
{
    string sk(key);
    if (StringMatch(sk, "STORM_DT"))
        m_dt = data;
    else if (StringMatch(sk, Tag_CellWidth))
        m_CellWidth = data;
    else if (StringMatch(sk, Tag_CellSize))
        m_nCells = (int) data;
    else if (StringMatch(sk, VAR_OMP_THREADNUM))
    {
        omp_set_num_threads((int) data);
    }
    else
        throw ModelException("SedBonNutrient_CH", "SetSingleData", "Parameter " + sk
                                                                   +
                                                                   " does not exist. Please contact the module developer.");
}

void SedBonNutrient_CH::GetValue(const char *key, float *value)
{
    string sk(key);
    if (StringMatch(sk, "SedBonPOutlet"))
    {
        map<int, vector<int> >::iterator it = m_reachLayers.end();
        it--;
        int reachId = it->second[0];
        int iLastCell = m_reachs[reachId].size() - 1;
        *value = m_SedBonP[reachId][iLastCell] * 1e6;        //kg/s   convert to mg/s

    }
    else if (StringMatch(sk, "SedBonAmmoniumOutlet"))
    {
        map<int, vector<int> >::iterator it = m_reachLayers.end();
        it--;
        int reachId = it->second[0];
        int iLastCell = m_reachs[reachId].size() - 1;
        *value = m_SedBonAmmonium[reachId][iLastCell] * 1e6;           //kg/s   convert to mg/s

    }
    else
        throw ModelException("SedBonNutrient_CH", "GetValue", "Output " + sk
                                                              +
                                                              " does not exist in the SedBonNutrient_CH module. Please contact the module developer.");
}

void SedBonNutrient_CH::Set1DData(const char *key, int n, float *data)
{
    //check the input data
    CheckInputSize(key, n);
    string sk(key);
    if (StringMatch(sk, "Slope"))
        m_Slope = data;
    else if (StringMatch(sk, "CHWIDTH"))
        m_chWidth = data;
    else if (StringMatch(sk, "D_SedBonPToCh"))
        m_SedBonPToCh = data;
    else if (StringMatch(sk, "D_SedBonAmmoniumToCh"))
        m_SedBonAmmoniumToCh = data;
    else if (StringMatch(sk, "STREAM_LINK"))
        m_streamLink = data;
    else if (StringMatch(sk, "FlowOut_Index_D8"))
        m_flowOutIndex = data;
    else
        throw ModelException("SedBonNutrient_CH", "Set1DData", "Parameter " + sk
                                                               +
                                                               " does not exist. Please contact the module developer.");

}

void SedBonNutrient_CH::Get1DData(const char *key, int *n, float **data)
{
    //string sk(key);
    //*n = m_nCells;
    ///*if (StringMatch(sk, "SEDSUBBASIN"))
    //{
    //*data = m_SedSubbasin;
    //}*/
    //if (StringMatch(sk, "DEP"))
    //{
    //	*data = m_depCh;
    //}
    //else if (StringMatch(sk, "DET"))
    //{
    //	*data = m_detCH;
    //}
    //else if (StringMatch(sk, "QSN"))
    //{
    //	*data = m_routQs;
    //}
    //else if (StringMatch(sk, "CAP"))
    //{
    //	*data = m_cap;
    //}
    //else
    //	throw ModelException("SedBonNutrient_CH", "Get1DData", "Output " + sk
    //	+ " does not exist in the SedBonNutrient_CH module. Please contact the module developer.");

}

void SedBonNutrient_CH::Set2DData(const char *key, int nrows, int ncols, float **data)
{
    string sk(key);
    if (StringMatch(sk, Tag_ReachParameter))
    {
        m_chNumber = ncols;
        m_reachId = data[0];
        m_streamOrder = data[1];
        m_reachDownStream = data[2];
        for (int i = 0; i < m_chNumber; i++)
            m_idToIndex[(int) m_reachId[i]] = i;

        m_reachUpStream.resize(m_chNumber);
        for (int i = 0; i < m_chNumber; i++)
        {
            int downStreamId = int(m_reachDownStream[i]);
            if (downStreamId == 0)
                continue;
            int downStreamIndex = m_idToIndex[downStreamId];
            m_reachUpStream[downStreamIndex].push_back(i);
        }

    }
    else if (StringMatch(sk, "FlowIn_Index_D8")) this->m_flowInIndex = data;
    else if (StringMatch(sk, "HCH")) this->m_ChannelWH = data;
    else if (StringMatch(sk, "QCH")) this->m_ChQkin = data;
    else
        throw ModelException("KinWavSed_CH", "Set2DData", "Parameter " + sk
                                                          + " does not exist. Please contact the module developer.");

}

void SedBonNutrient_CH::Get2DData(const char *key, int *nRows, int *nCols, float ***data)
{

}

void  SedBonNutrient_CH::initialOutputs()
{
    //allocate the output variable
    if (this->m_nCells <= 0)
        throw ModelException("SedBonNutrient_CH", "initialOutputs",
                             "The cell number of the input can not be less than zero.");
    if (this->m_chNumber <= 0)
        throw ModelException("DissolveNutrient_CH", "initialOutputs",
                             "The channel number of the input can not be less than zero.");

    if (m_SedBonP_KG == NULL)
    {
        // find source cells the reaches
        m_sourceCellIds = new int[m_chNumber];
        for (int i = 0; i < m_chNumber; ++i)
            m_sourceCellIds[i] = -1;
        for (int i = 0; i < m_nCells; i++)
        {
            if (FloatEqual(m_streamLink[i], NODATA_VALUE))
                continue;
            int reachId = (int) m_streamLink[i];
            bool isSource = true;
            for (int k = 1; k <= (int) m_flowInIndex[i][0]; ++k)
            {
                int flowInId = (int) m_flowInIndex[i][k];
                int flowInReachId = (int) m_streamLink[flowInId];
                if (flowInReachId == reachId)
                {
                    isSource = false;
                    break;
                }
            }

            if ((int) m_flowInIndex[i][0] == 0)
                isSource = true;

            if (isSource)
            {
                int reachIndex = m_idToIndex[reachId];
                m_sourceCellIds[reachIndex] = i;
            }
        }
        // get the cells in reaches according to flow direction
        for (int iCh = 0; iCh < m_chNumber; iCh++)
        {
            int iCell = m_sourceCellIds[iCh];
            int reachId = (int) m_streamLink[iCell];
            while ((int) m_streamLink[iCell] == reachId)
            {
                m_reachs[iCh].push_back(iCell);
                iCell = (int) m_flowOutIndex[iCell];
            }
        }

        if (m_reachLayers.empty())
        {
            for (int i = 0; i < m_chNumber; i++)
            {
                int order = (int) m_streamOrder[i];
                m_reachLayers[order].push_back(i);
            }
        }

        m_ChVol = new float *[this->m_chNumber];

        m_SedBonP = new float *[this->m_chNumber];
        m_SedBonAmmonium = new float *[this->m_chNumber];
        m_SedBonP_KG = new float *[this->m_chNumber];
        m_SedBonAmmonium_KG = new float *[this->m_chNumber];
        for (int i = 0; i < m_chNumber; ++i)
        {

            int n = m_reachs[i].size();
            m_ChVol[i] = new float[n];
            m_SedBonP[i] = new float[n];
            m_SedBonAmmonium[i] = new float[n];
            m_SedBonP_KG[i] = new float[n];
            m_SedBonAmmonium_KG[i] = new float[n];
            for (int j = 0; j < n; ++j)
            {
                m_ChVol[i][j] = 0.f;
                m_SedBonP[i][j] = 0.f;
                m_SedBonAmmonium[i][j] = 0.f;
                m_SedBonP_KG[i][j] = 0.f;
                m_SedBonAmmonium_KG[i][j] = 0.f;
            }
        }
    }
}

string SedBonNutrient_CH::toString(float value)
{
    char s[20];
    sprintf_s(s, 20, "%f", value);
    return string(s);
}

void SedBonNutrient_CH::WaterVolumeCalc(int iReach, int iCell, int id)
{
    float slope, DX, wh;
    slope = atan(m_Slope[id]);
    DX = m_CellWidth / cos(slope);
    wh = m_ChannelWH[iReach][iCell] /
         1000;  //mm -> m   : m_ChannelWH[iReach][iCell] => "HCH" from the channel routing module
    m_ChVol[iReach][iCell] = DX * m_chWidth[id] * wh;  // m3
}

float SedBonNutrient_CH::simpleSedCalc(float Qn, float Qin, float Sin, float dt, float vol, float sed)
{
    float Qsn = 0;
    float totsed = sed + Sin * dt;  // add upstream sed to sed present in cell
    float totwater = vol + Qin * dt;   // add upstream water to volume water in cell
    if (totwater <= 1e-10)
        return (Qsn);
    Qsn = min(totsed / dt, Qn * totsed / totwater);
    return (Qsn); // sedoutflow is new concentration * new out flux

}

void SedBonNutrient_CH::GetNutrientInFlow(int iReach, int iCell, int id)
{
    //sum the upstream overland flow
    float Qin = 0.f;
    float PUp = 0.0f;
    float NH4Up = 0.0f;
    float NO3Up = 0.0f;
    if (iCell == 0)// inflow of this cell is the last cell of the upstream reach
    {
        for (int i = 0; i < m_reachUpStream[iReach].size(); ++i)
        {
            int upReachId = m_reachUpStream[iReach][i];
            if (upReachId >= 0)
            {
                int upCellsNum = m_reachs[upReachId].size();
                int upCellId = m_reachs[upReachId][upCellsNum - 1];
                Qin += m_ChQkin[upReachId][upCellsNum - 1];
                PUp += m_SedBonP[upReachId][upCellsNum - 1];
                NH4Up += m_SedBonAmmonium[upReachId][upCellsNum - 1];
            }
        }
    }
    else
    {
        Qin = m_ChQkin[iReach][iCell - 1];
        PUp = m_SedBonP[iReach][iCell - 1];   //the iCell-1 is the prior of the iCell
        NH4Up = m_SedBonAmmonium[iReach][iCell - 1];
    }

    this->WaterVolumeCalc(iReach, iCell, id);
    float WtVol = m_ChVol[iReach][iCell];

    m_SedBonP_KG[iReach][iCell] = m_SedBonPToCh[id];
    m_SedBonAmmonium_KG[iReach][iCell] = m_SedBonAmmoniumToCh[id];

    m_SedBonP[iReach][iCell] = simpleSedCalc(m_ChQkin[iReach][iCell], Qin, PUp, m_dt, WtVol,
                                             m_SedBonP_KG[iReach][iCell]);
    m_SedBonAmmonium[iReach][iCell] = simpleSedCalc(m_ChQkin[iReach][iCell], Qin, NH4Up, m_dt, WtVol,
                                                    m_SedBonAmmonium_KG[iReach][iCell]);

    float tem = PUp + m_SedBonP_KG[iReach][iCell] / m_dt;    //kg/s
    // no more SedBonP outflow than total SedBonP in cell
    m_SedBonP[iReach][iCell] = min(m_SedBonP[iReach][iCell], tem);
    //m_routQs[id] = m_Qsn[iReach][iCell];
    tem = PUp * m_dt + m_SedBonP_KG[iReach][iCell] - m_SedBonP[iReach][iCell] * m_dt;
    // new SedBonP volume based on all fluxes or SedBonP present
    m_SedBonP_KG[iReach][iCell] = max(0.0f, tem);

    tem = NH4Up + m_SedBonAmmonium_KG[iReach][iCell] / m_dt;
    m_SedBonAmmonium[iReach][iCell] = min(m_SedBonAmmonium[iReach][iCell], tem);
    tem = NH4Up * m_dt + m_SedBonAmmonium_KG[iReach][iCell] - m_SedBonAmmonium[iReach][iCell] * m_dt;
    m_SedBonAmmonium_KG[iReach][iCell] = max(0.0f, tem);

}

int SedBonNutrient_CH::Execute()
{
    this->CheckInputData();

    initialOutputs();
    map<int, vector<int> >::iterator it;
    for (it = m_reachLayers.begin(); it != m_reachLayers.end(); it++)
    {
        // There are not any flow relationship within each routing layer.
        // So parallelization can be done here.
        int nReaches = it->second.size();
        // the size of m_reachLayers (map) is equal to the maximum stream order
#pragma omp parallel for
        for (int i = 0; i < nReaches; ++i)
        {
            int reachIndex = it->second[i]; // index in the array
            vector<int> &vecCells = m_reachs[reachIndex];
            int n = vecCells.size();
            for (int iCell = 0; iCell < n; ++iCell)
            {
                GetNutrientInFlow(reachIndex, iCell, vecCells[iCell]);
            }
        }
    }

    return 0;
}