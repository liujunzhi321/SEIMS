#include "BMPReach.h"
#include "BMPText.h"
#include "DBManager.h"
#include "ModelException.h"
#include "util.h"
#include "utils.h"
#include <vector>
#include <sstream>

using namespace MainBMP;

BMPReach::BMPReach(string bmpDatabasePath,
                   string parameterTableName,
                   int bmpId,
                   string bmpName,
                   int reachId,
                   int reachStructurId) : BMPBase(bmpDatabasePath, bmpId, bmpName, BMP_TYPE_REACH, parameterTableName)
{
    m_reachId = reachId;
    m_reachStructureId = reachStructurId;

    //load parameters
    m_numericParameterNum = -1;
    m_numericParameters = NULL;
    loadParameters(parameterTableName, reachStructurId);
}

BMPReach::~BMPReach(void)
{
    if (m_numericParameters != NULL) delete[] m_numericParameters;
}

void BMPReach::loadParameters(string parameterTableName, int reachStructurId)
{
    string bmpDatabase = m_bmpDatabasePath + BMP_DATABASE_NAME;
    if (!DBManager::IsTableExist(bmpDatabase, parameterTableName))
        throw ModelException("BMPReach", "loadParameters", "The BMP database '" + bmpDatabase +
                                                           "' does not exist or the there is not a table named '" +
                                                           parameterTableName + "' in BMP database.");

    //read corresponding data
    DBManager db;
    db.Open(bmpDatabase);
    try
    {
        string tableInfoSQL = "pragma table_info(" + parameterTableName + ")";
        slTable *table = db.Load(tableInfoSQL);

        //get all numeric and text columns
        map<int, string> textColumns;
        map<int, bool> numericColumns;
        int methodIndex = -1;
        int sedimentMethodIndex = -1;
        int nutrientMethodIndex = -1;
        for (int i = 0; i < table->nRows; i++)
        {
            string type = table->FieldValue(i + 1, 2);
            if (!StringMatch(type, "TEXT"))
            {
                if (StringMatch(type, "DATE") || StringMatch(type, "DATETIME"))
                    numericColumns[i] = true;
                else
                    numericColumns[i] = false;
            }
            else
            {
                textColumns[i] = table->FieldValue(i + 1, 1);
                if (StringMatch(textColumns[i], RESERVOIR_FLOW_ROUTING_METHOD_COLUMN_NAME))
                    methodIndex = i;    //flow routing method
                if (StringMatch(textColumns[i], RESERVOIR_SEDIMENT_ROUTING_METHOD_COLUMN_NAME))
                    sedimentMethodIndex = i;//sediment routing method
                if (StringMatch(textColumns[i], RESERVOIR_NUTRIENT_ROUTING_METHOD_COLUMN_NAME))
                    nutrientMethodIndex = i;//nutrient routing method
            }
        }
        delete table;

        //get the data for the specific id
        ostringstream oss;
        oss << "select * from " << parameterTableName << " where id =" << reachStructurId;
        string selectSQL = oss.str();
        table = db.Load(selectSQL);
        if (table->nRows == 0)
        {
            oss.clear();
            oss << "There is not a " << parameterTableName << " whose id is " << reachStructurId << ".";
            throw ModelException("BMPReach", "loadParameters", oss.str());
        }

        //get numeric values
        m_numericParameterNum = int(numericColumns.size());
        if (m_bmpId == BMP_TYPE_RESERVOIR)
        {
            if (methodIndex == -1)
                throw ModelException("BMPReach", "loadParameters", "Can't find flow routing method column in table " +
                                                                   m_parameterTableName + ".");
            if (sedimentMethodIndex == -1)
                throw ModelException("BMPReach", "loadParameters",
                                     "Can't find sediment routing method column in table " + m_parameterTableName +
                                     ".");
            if (nutrientMethodIndex == -1)
                throw ModelException("BMPReach", "loadParameters",
                                     "Can't find nutrient routing method column in table " + m_parameterTableName +
                                     ".");

            m_numericParameterNum += 3;
        }


        if (m_numericParameterNum > 0)
        {
            int index = 0;
            m_numericParameters = new float[m_numericParameterNum];
            map<int, bool>::iterator it;
            for (it = numericColumns.begin(); it != numericColumns.end(); it++)
            {
                string value = table->FieldValue(1, it->first);
                float temp = 0.0f;
                if (it->second)    //date
                {
                    utils util;
                    m_operationDate = util.ConvertToTime(value, "%4d-%2d-%2d", false);
                    temp = float(m_operationDate / 86400);
                }
                else
                {
                    temp = float(atof(value.c_str()));
                    if (it->first == BMP_REACH_X_INDEX) m_x = temp;
                    if (it->first == BMP_REACH_Y_INDEX) m_y = temp;
                }

                m_numericParameters[index] = temp;
                index++;
            }
        }

        //get text values
        map<int, string>::iterator it2;
        for (it2 = textColumns.begin(); it2 != textColumns.end(); it2++)
        {
            m_textParameters[it2->second] = table->FieldValue(1, it2->first);
        }

        delete table;
        db.Close();
    }
    catch (...)
    {
        db.Close();
        throw;
    }
}

void BMPReach::loadStructureSpecificParameter() { }

void BMPReach::loadTimeSeriesData(string databasePath, time_t startTime, time_t endTime, int interval)
{
    if (!HasTimeSeriesData()) return;

    string tableName = TimeSeriesDataTableName();
    if (tableName.length() == 0)
    {
        ostringstream oss;
        oss << "The data source for " << m_bmpName << m_reachStructureId << "is empty.";
        throw ModelException("BMPReach", "loadTimeSeriesData", oss.str());
    }

    string hydroClimatePath = databasePath + File_HydroClimateDB;
    if (!DBManager::IsTableExist(hydroClimatePath, tableName))
        throw ModelException("BMPReach", "loadTimeSeriesData", "The hydroclimate database '" + hydroClimatePath +
                                                               "' does not exist or the there is not a table named '" +
                                                               tableName + "' in hydroclimate database.");

    DBManager db;
    db.Open(hydroClimatePath);
    try
    {
        utils util;
        string sStart = util.ConvertToString(&startTime);
        string sEnd = util.ConvertToString(&endTime);
        string strSQL =
                "SELECT * FROM " + tableName + " WHERE [DATE] >= DATE('" + sStart + "') AND [DATE] <= DATE('" + sEnd +
                "')";

        slTable *tbl = db.Load(strSQL);
        if (tbl->nRows > 0)
        {
            int dateColIndex = 1;
            for (int j = 0; j < tbl->nCols; j++)
            {
                string col = tbl->FieldValue(0, j);
                int colType = TimeSeriesDataName2Type(col);
                if (colType == BMP_REACH_UNKONWN_TYPE) continue;

                for (int i = 1; i <= tbl->nRows; i++)
                {
                    time_t t = util.ConvertToTime(tbl->FieldValue(i, dateColIndex), "%4d-%2d-%2d", false);
                    float value = float(atof(tbl->FieldValue(i, j).c_str()));
                    m_timeSerieseData[colType][t] = value;
                }
            }
        }

        delete tbl;
        db.Close();
    }
    catch (...)
    {
        db.Close();
        throw;
    }
}

int    BMPReach::TimeSeriesDataName2Type(string name)
{
    if (StringMatch(name, BMP_REACH_FLOW_NAME)) return BMP_REACH_FLOW_TYPE;
    if (StringMatch(name, BMP_REACH_SEDIMENT_NAME)) return BMP_REACH_SEDIMENT_TYPE;
    if (StringMatch(name, BMP_REACH_ORGANICN_NAME)) return BMP_REACH_ORGANICN_TYPE;
    if (StringMatch(name, BMP_REACH_ORGANICP_NAME)) return BMP_REACH_ORGANICP_TYPE;
    if (StringMatch(name, BMP_REACH_NO3_NAME)) return BMP_REACH_NO3_TYPE;
    if (StringMatch(name, BMP_REACH_NH3_NAME)) return BMP_REACH_NH3_TYPE;
    if (StringMatch(name, BMP_REACH_NO2_NAME)) return BMP_REACH_NO2_TYPE;
    if (StringMatch(name, BMP_REACH_MINERALP_NAME)) return BMP_REACH_MINERALP_TYPE;
    return BMP_REACH_UNKONWN_TYPE;
}

string    BMPReach::TimeSeriesDataType2Name(int type)
{
    if (type == BMP_REACH_FLOW_TYPE) return BMP_REACH_FLOW_NAME;
    if (type == BMP_REACH_SEDIMENT_TYPE) return BMP_REACH_SEDIMENT_NAME;
    if (type == BMP_REACH_ORGANICN_TYPE) return BMP_REACH_ORGANICN_NAME;
    if (type == BMP_REACH_ORGANICP_TYPE) return BMP_REACH_ORGANICP_NAME;
    if (type == BMP_REACH_NO3_TYPE) return BMP_REACH_NO3_NAME;
    if (type == BMP_REACH_NH3_TYPE) return BMP_REACH_NH3_NAME;
    if (type == BMP_REACH_NO2_TYPE) return BMP_REACH_NO2_NAME;
    if (type == BMP_REACH_MINERALP_TYPE) return BMP_REACH_MINERALP_NAME;
    return "";
}

float BMPReach::getTimeSeriesData(int dataType, time_t t)
{
    map<int, map<time_t, float> >::iterator it3 = m_timeSerieseData.find(dataType);
    if (it3 == m_timeSerieseData.end()) return 0.0f;

    map<time_t, float>::iterator it2 = it3->second.find(t);
    if (it2 == it3->second.end()) return 0.0f;

    return it2->second;
}

string BMPReach::TimeSeriesDataTableName()
{
    map<string, string>::iterator it;
    for (it = m_textParameters.begin(); it != m_textParameters.end(); it++)
    {
        if (StringMatch(it->first, "FILE") || StringMatch(it->first, "TABLENAME")) return it->second;
    }
    return "";
}

bool BMPReach::HasTimeSeriesData() { return false; }

string BMPReach::DataSource()
{
    map<string, string>::iterator it;
    for (it = m_textParameters.begin(); it != m_textParameters.end(); it++)
    {
        if (StringMatch(it->first, "FILE") || StringMatch(it->first, "TABLENAME")) return it->second;
    }
    return "";
}

void BMPReach::Dump(ostream *fs)
{
    if (fs == NULL) return;

    BMPBase::Dump(fs);

    utils util;

    //basic information
    *fs << "*** properties ***" << endl;
    for (int i = 0; i < m_numericParameterNum; i++)
    {
        *fs << right << fixed << setw(15) << setfill(' ') << setprecision(8) << m_numericParameters[i] << endl;
    }
    map<string, string>::iterator it;
    for (it = m_textParameters.begin(); it != m_textParameters.end(); it++)
    {
        *fs << it->first << ":" << it->second << endl;
    }

    //time series data
    if (!HasTimeSeriesData()) return;

    *fs << "*** Time Series Data ***" << endl;
    map<int, map<time_t, float> >::iterator it3;
    for (it3 = m_timeSerieseData.begin(); it3 != m_timeSerieseData.end(); it3++)
    {
        *fs << "*** " << TimeSeriesDataType2Name(it3->first) << " ***" << endl;
        map<time_t, float>::iterator it2;
        for (it2 = it3->second.begin(); it2 != it3->second.end(); it2++)
        {
            *fs << util.ConvertToString(&(it2->first)) <<
            right << fixed << setw(15) << setfill(' ') <<
            setprecision(4) << it2->second << endl;
        }
    }
}
