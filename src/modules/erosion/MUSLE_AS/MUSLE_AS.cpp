#include <cmath>
#include "MUSLE_AS.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <math.h>
#include <omp.h>
#include <map>

MUSLE_AS::MUSLE_AS(void) : m_nCells(-1), m_cellWidth(-1.f), m_nsub(-1), m_nSoilLayers(-1),
	                       m_usle_c(NULL), m_usle_p(NULL), m_usle_k(NULL), m_usle_ls(NULL),
                           m_flowacc(NULL), m_slope(NULL), m_streamLink(NULL), m_slopeForPq(NULL),
                           m_snowAccumulation(NULL), m_surfaceRunoff(NULL),
                           m_sedimentYield(NULL)
{
}

MUSLE_AS::~MUSLE_AS(void)
{
    if (m_sedimentYield != NULL) Release1DArray(m_sedimentYield);
    
    if (m_usle_ls != NULL) Release1DArray(m_usle_ls);
    if (m_slopeForPq != NULL) Release1DArray(m_slopeForPq);
}

bool MUSLE_AS::CheckInputData(void)
{
    if (m_nCells <= 0)
        throw ModelException(MID_MUSLE_AS, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    if (m_cellWidth <= 0)
        throw ModelException(MID_MUSLE_AS, "CheckInputData", "The cell width can not be less than zero.");
	if (m_depRatio == NODATA_VALUE)
		throw ModelException(MID_MUSLE_AS, "CheckInputData", "The deposition ratio can not be nodata.");
    if (m_usle_c == NULL) throw ModelException(MID_MUSLE_AS, "CheckInputData", "The factor C can not be NULL.");
    if (m_usle_k == NULL) throw ModelException(MID_MUSLE_AS, "CheckInputData", "The factor K can not be NULL.");
    if (m_usle_p == NULL) throw ModelException(MID_MUSLE_AS, "CheckInputData", "The factor P can not be NULL.");
    if (m_flowacc == NULL)
        throw ModelException(MID_MUSLE_AS, "CheckInputData", "The flow accumulation can not be NULL.");
    if (m_slope == NULL) throw ModelException(MID_MUSLE_AS, "CheckInputData", "The slope can not be NULL.");
    if (m_snowAccumulation == NULL)
        throw ModelException(MID_MUSLE_AS, "CheckInputData", "The snow accumulation can not be NULL.");
    if (m_surfaceRunoff == NULL)
        throw ModelException(MID_MUSLE_AS, "CheckInputData", "The surface runoff can not be NULL.");
    if (m_streamLink == NULL)
        throw ModelException(MID_MUSLE_AS, "CheckInputData", "The parameter: STREAM_LINK has not been set.");
    return true;
}

void MUSLE_AS::initialOutputs()
{
    if (m_nCells <= 0)
        throw ModelException(MID_MUSLE_AS, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    // allocate the output variables
    if (m_nsub <= 0)// TODO prepared in parameter tables.
    {
        map<int, int> subs;
        for (int i = 0; i < m_nCells; i++)
        {
            subs[int(m_subbasin[i])] += 1;
        }
        m_nsub = subs.size();
    }
   
    if (m_sedimentYield == NULL)
		Initialize1DArray(m_nCells, m_sedimentYield, 0.f);

    if (m_usle_ls == NULL)
    {
        float constant = pow(22.13f, 0.4f);
        m_usle_ls = new float[m_nCells];
        m_slopeForPq = new float[m_nCells];
#pragma omp parallel for
        for (int i = 0; i < m_nCells; i++)
        {
            if (m_usle_c[i] < 0.001f)
                m_usle_c[i] = 0.001f;
            if (m_usle_c[i] > 1.0f)
                m_usle_c[i] = 1.0f;
            float lambda_i1 = m_flowacc[i] * m_cellWidth;  // m
            float lambda_i = lambda_i1 + m_cellWidth;
            float L = pow(lambda_i, 1.4f) - pow(lambda_i1, 1.4f); // m^1.4
            L /= m_cellWidth * constant;  /// m^1.4 / m^0.4 = m

            float S = pow(sin(atan(m_slope[i])) / 0.0896f, 1.3f);

            m_usle_ls[i] = L * S;//equation 3 in memo, LS factor

            m_slopeForPq[i] = pow(m_slope[i] * 1000.f, 0.16f);
        }
    }
    m_cellAreaKM = pow(m_cellWidth / 1000.f, 2.0f);
    m_cellAreaKM1 = 3.79f * pow(m_cellAreaKM, 0.7f);
    m_cellAreaKM2 = 0.903f * pow(m_cellAreaKM, 0.017f);
}

float MUSLE_AS::getPeakRunoffRate(int cell)
{
    return m_cellAreaKM1 * m_slopeForPq[cell] *
           pow(m_surfaceRunoff[cell] / 25.4f, m_cellAreaKM2); //equation 2 in memo, peak flow
}

int MUSLE_AS::Execute()
{
    CheckInputData();

    initialOutputs();

#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++)
    {
        if (m_surfaceRunoff[i] < 0.0001f || m_streamLink[i] > 0)
            m_sedimentYield[i] = 0.0f;
        else
        {
            float q = getPeakRunoffRate(i); //equation 2 in memo, peak flow
            float Y = 11.8f * pow(m_surfaceRunoff[i] * m_cellAreaKM * 1000.0f * q, 0.56f)
                      * m_usle_k[i][0] * m_usle_ls[i] * m_usle_c[i] * m_usle_p[i];    //equation 1 in memo, sediment yield

            if (m_snowAccumulation[i] > 0.0001f)
                Y /= exp(3.0f * m_snowAccumulation[i] / 25.4f);  //equation 4 in memo, the snow pack effect
            m_sedimentYield[i] = Y * 1000.f; /// kg
        }
    }

    return 0;
}


bool MUSLE_AS::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
        throw ModelException(MID_MUSLE_AS, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    if (m_nCells != n)
    {
        if (m_nCells <= 0) m_nCells = n;
        else
            throw ModelException(MID_MUSLE_AS, "CheckInputSize", "Input data for " + string(key) +
                                                                 " is invalid. All the input data should have same size.");
    }
    return true;
}

void MUSLE_AS::SetValue(const char *key, float data)
{
    string sk(key);
    if (StringMatch(sk, Tag_CellWidth))
        m_cellWidth = data;
    else if (StringMatch(sk, Tag_CellSize))
        m_nCells = (int) data;
	else if (StringMatch(sk, VAR_DEPRATIO))
		m_depRatio = data;
    else if (StringMatch(sk, VAR_OMP_THREADNUM))
    {
        omp_set_num_threads((int) data);
    }
    else
        throw ModelException(MID_MUSLE_AS, "SetValue", "Parameter " + sk
                                                       +
                                                       " does not exist in current module. Please contact the module developer.");
}

void MUSLE_AS::Set1DData(const char *key, int n, float *data)
{

    CheckInputSize(key, n);

    //set the value
    string s(key);

    if (StringMatch(s, VAR_USLE_C)) m_usle_c = data;
    else if (StringMatch(s, VAR_USLE_P)) m_usle_p = data;
    else if (StringMatch(s, VAR_ACC)) m_flowacc = data;
    else if (StringMatch(s, VAR_SLOPE)) m_slope = data;
    else if (StringMatch(s, VAR_SUBBSN)) m_subbasin = data;
    else if (StringMatch(s, VAR_FLOW_OL)) m_surfaceRunoff = data;
    else if (StringMatch(s, VAR_SNAC)) m_snowAccumulation = data;
    else if (StringMatch(s, VAR_STREAM_LINK))
        m_streamLink = data;
    else
        throw ModelException(MID_MUSLE_AS, "Set1DData", "Parameter " + s +
                                                       " does not exist in current module. Please contact the module developer.");
}
void MUSLE_AS::Set2DData(const char *key, int nRows, int nCols, float **data)
{
	string s(key);
	CheckInputSize(key, nRows);
	m_nSoilLayers = nCols;
	if (StringMatch(s, VAR_USLE_K))
	{
		this->m_usle_k = data;
	}
	else
		throw ModelException(MID_MUSLE_AS, "Set2DData", "Parameter " + s +
		" does not exist in current module. Please contact the module developer.");
}
void MUSLE_AS::GetValue(const char *key, float *value)
{
	string s(key);
    throw ModelException(MID_MUSLE_AS, "GetValue",
                             "Result " + s + " does not exist in current module. Please contact the module developer.");
}

void MUSLE_AS::Get1DData(const char *key, int *n, float **data)
{
    string sk(key);
    if (StringMatch(sk, VAR_SOER)) *data = m_sedimentYield;
    else if (StringMatch(sk, VAR_USLE_LS)) *data = m_usle_ls;
    else
        throw ModelException(MID_MUSLE_AS, "Get1DData", "Result " + sk +
                                                        " does not exist in current module. Please contact the module developer.");
    *n = m_nCells;
}


