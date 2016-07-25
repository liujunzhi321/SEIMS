#include "BMPReachReservoir.h"

using namespace MainBMP;

BMPReachReservoir::BMPReachReservoir(
        string bmpDatabasePath,
        string parameterTableName,
        int reachId,
        int ReservoirId) : BMPReach(
        bmpDatabasePath,
        parameterTableName,
        BMP_TYPE_RESERVOIR,
        BMP_NAME_RESERVOIR,
        reachId,
        ReservoirId)
{
    m_ratingCurveData = NULL;
    //m_operationSchedualData = NULL;

    getRoutingMethod(parameterTableName);
    loadStructureSpecificParameter();
}

void BMPReachReservoir::getRoutingMethod(string parameterTableName)
{
    ostringstream os;
    map<string, string>::iterator it;

    //flow routing
    it = m_textParameters.find(RESERVOIR_FLOW_ROUTING_METHOD_COLUMN_NAME);
    if (it == m_textParameters.end())
    {
        os << "Can't find column " << RESERVOIR_FLOW_ROUTING_METHOD_COLUMN_NAME
        << " in table " << parameterTableName << ".";
        throw ModelException("BMPReachReservoir", "getRoutingMethod", os.str());
    }

    int method = FlowRoutingMethod2Int(it->second);
    if (method > -1) m_methodFlowInt = method;
    else
        throw ModelException("BMPReachReservoir", "getRoutingMethod",
                             "Unknown reservoir flow routing method:" + it->second);

    //sediment routing
    it = m_textParameters.find(RESERVOIR_SEDIMENT_ROUTING_METHOD_COLUMN_NAME);
    if (it == m_textParameters.end())
    {
        os << "Can't find column " << RESERVOIR_SEDIMENT_ROUTING_METHOD_COLUMN_NAME
        << " in table " << parameterTableName << ".";
        throw ModelException("BMPReachReservoir", "getRoutingMethod", os.str());
    }

    method = SedimentRoutingMethod2Int(it->second);
    if (method > -1) m_methodSedimentInt = method;
    else
        throw ModelException("BMPReachReservoir", "getRoutingMethod",
                             "Unknown reservoir sediment routing method:" + it->second);

    //nutrient routing
    it = m_textParameters.find(RESERVOIR_NUTRIENT_ROUTING_METHOD_COLUMN_NAME);
    if (it == m_textParameters.end())
    {
        os << "Can't find column " << RESERVOIR_NUTRIENT_ROUTING_METHOD_COLUMN_NAME
        << " in table " << parameterTableName << ".";
        throw ModelException("BMPReachReservoir", "getRoutingMethod", os.str());
    }

    method = NutrientMethod2Int(it->second);
    if (method > -1) m_methodNutrientInt = method;
    else
        throw ModelException("BMPReachReservoir", "getRoutingMethod",
                             "Unknown reservoir nutrient routing method:" + it->second);

    //add it to the parameter array
    m_numericParameters[m_numericParameterNum - 3] = float(m_methodFlowInt);        //save the method information
    m_numericParameters[m_numericParameterNum - 2] = float(m_methodSedimentInt);
    m_numericParameters[m_numericParameterNum - 1] = float(m_methodNutrientInt);
}

BMPReachReservoir::~BMPReachReservoir(void)
{
    if (m_ratingCurveData != NULL) delete m_ratingCurveData;
    //if(m_operationSchedualData != NULL) delete m_operationSchedualData;
}

string BMPReachReservoir::ColumnName() { return BMP_NAME_RESERVOIR; }

bool BMPReachReservoir::HasTimeSeriesData()
{
    if (m_methodFlowInt == RESERVOIR_FLOW_ROUTING_DAILY_OUTFLOW ||
        m_methodFlowInt == RESERVOIR_FLOW_ROUTING_MONTHLY_OUTFLOW)
        return true;
    return false;
}

void BMPReachReservoir::loadStructureSpecificParameter()
{
    if (m_methodFlowInt == RESERVOIR_FLOW_ROUTING_RATING_CURVE ||
        m_methodFlowInt == RESERVOIR_FLOW_ROUTING_TARGET_RELEASE_RATE)
    {
        if (DataSource().length() == 0)
        {
            ostringstream oss;
            oss << "The data source for " << m_bmpName << m_reachStructureId << "is empty.";
            throw ModelException("BMPReachReservoir", "loadStructureSpecificParameter", oss.str());
        }

        utils util;
        string fileName = m_bmpDatabasePath + DataSource();
        if (!util.FileExists(fileName))
            throw ModelException("BMPReachReservoir", "loadStructureSpecificParameter",
                                 "The file '" + fileName + "' does not exist.");

        if (m_methodFlowInt == RESERVOIR_FLOW_ROUTING_RATING_CURVE)    //rating curve file
        {
            m_ratingCurveData = new clsInterpolationWeightData(fileName);
        }
        else                                //operation schedual file
        {
            //m_operationSchedualData = new clsSimpleTxtData(fileName);
        }
    }
}

int BMPReachReservoir::NutrientMethod2Int(string method)
{
    return 1;
}

int BMPReachReservoir::SedimentRoutingMethod2Int(string method)
{
    if (StringMatch(method, RESERVOIR_SEDIMENT_ROUTING_NAME_MASS_BALANCE))
        return RESERVOIR_SEDIMENT_ROUTING_MASS_BALANCE;
    else
        return RESERVOIR_SEDIMENT_ROUTING_MASS_UNKONWN;
}

int BMPReachReservoir::FlowRoutingMethod2Int(string method)
{
    if (StringMatch(method, RESERVOIR_FLOW_ROUTING_NAME_RATING_CURVE))
        return RESERVOIR_FLOW_ROUTING_RATING_CURVE;
    else if (StringMatch(method, RESERVOIR_FLOW_ROUTING_NAME_DAILY_OUTFLOW))
        return RESERVOIR_FLOW_ROUTING_DAILY_OUTFLOW;
    else if (StringMatch(method, RESERVOIR_FLOW_ROUTING_NAME_MONTHLY_OUTFLOW))
        return RESERVOIR_FLOW_ROUTING_MONTHLY_OUTFLOW;
    else if (StringMatch(method, RESERVOIR_FLOW_ROUTING_NAME_ANUNAL_RELEASE_RATE))
        return RESERVOIR_FLOW_ROUTING_ANUNAL_RELEASE_RATE;
    else if (StringMatch(method, RESERVOIR_FLOW_ROUTING_NAME_TARGET_RELEASE_RATE))
        return RESERVOIR_FLOW_ROUTING_TARGET_RELEASE_RATE;
    else return RESERVOIR_FLOW_ROUTING_NAME_UNKNOWN;
}

clsInterpolationWeightData *BMPReachReservoir::RatingCurve() { return m_ratingCurveData; }

clsSimpleTxtData *BMPReachReservoir::OperationSchedual() { return m_operationSchedualData; }

void BMPReachReservoir::Dump(ostream *fs)
{
    if (fs == NULL) return;
    BMPReach::Dump(fs);

    if (m_ratingCurveData != NULL)
    {
        *fs << "*** Rating Curve ***" << endl;
        m_ratingCurveData->dump(fs);
    }

    if (m_operationSchedualData != NULL)
    {
        *fs << "*** Operation Schedual ***" << endl;
        //m_operationSchedualData->dump(fs);
    }
}
