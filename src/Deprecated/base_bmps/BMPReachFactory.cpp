#include "BMPReachFactory.h"

using namespace MainBMP;

BMPReachFactory::BMPReachFactory(int scenarioId, int bmpId, int bmpType,
                                 string distribution, string parameter) : BMPFactory(scenarioId, bmpId, bmpType,
                                                                                     distribution, parameter)
{

}


BMPReachFactory::~BMPReachFactory(void)
{
    map<int, BMPReach *>::iterator it;
    for (it = m_bmpReachs.begin(); it != m_bmpReachs.end(); it++)
    {
        if (it->second != NULL) delete it->second;
    }
}

void BMPReachFactory::loadBMP(string bmpDatabasePath)
{
    if (m_bmpType != BMP_TYPE_REACH)
        throw ModelException("BMPReachFactory", "loadBMP", "Error BMP type for BMP reach!");

    //read all the reaches in reach_BMP table
    string bmpDatabase = bmpDatabasePath + BMP_DATABASE_NAME;
    if (!DBManager::IsTableExist(bmpDatabase, m_distribution))
        throw ModelException("BMPReachFactory", "loadBMP", "The BMP database '" + bmpDatabase +
                                                           "' does not exist or the there is not a table named '" +
                                                           m_distribution + "' in BMP database.");


    DBManager db;
    db.Open(bmpDatabase);
    try
    {
        utils util;
        ostringstream str;
        str << "SELECT scenario,reach," << BMPBase::ReachBMPColumnNameFromBMPID(m_bmpId)
        << " FROM " << m_distribution
        << " where "
        << "scenario = " << m_scenarioId << " or "
        << "scenario = " << BASE_SCENARIO_ID
        << " order by scenario DESC";

        string strSQL = str.str();
        slTable *tbl = db.Load(strSQL);
        if (tbl->nRows > 0)
        {
            for (int i = 1; i <= tbl->nRows; i++)
            {
                int reachId = atoi(tbl->FieldValue(i, 1).c_str());
                if (ReachBMP(reachId) == NULL)        //base scenario will be overrided by other scenario
                {
                    int reachConstructureId = atoi(tbl->FieldValue(i, 2).c_str());
                    if (reachConstructureId > 0)        //0 means no reach constructure
                    {
                        if (m_bmpType == BMP_TYPE_REACH)
                        {
                            BMPReach *reach = NULL;
                            switch (m_bmpId)
                            {
                                case BMP_TYPE_POINTSOURCE:
                                    reach = new BMPReachPointSource(bmpDatabasePath, m_parameter, reachId,
                                                                    reachConstructureId);
                                    break;
                                case BMP_TYPE_FLOWDIVERSION_STREAM:
                                    reach = new BMPReachFlowDiversion(bmpDatabasePath, m_parameter, reachId,
                                                                      reachConstructureId);
                                    break;
                                case BMP_TYPE_RESERVOIR:
                                    reach = new BMPReachReservoir(bmpDatabasePath, m_parameter, reachId,
                                                                  reachConstructureId);
                                    break;
                                    //case BMP_TYPE_RIPARIANWETLAND:
                                    //	reach = new BMPReachPointSource(bmpDatabasePath,m_parameter,reachId,reachConstructureId);
                                    //	break;
                                    //case BMP_TYPE_RIPARIANBUFFER:
                                    //	reach = new BMPReachPointSource(bmpDatabasePath,m_parameter,reachId,reachConstructureId);
                                    //	break;
                                default:
                                    break;
                            }
                            if (reach != NULL) m_bmpReachs[reachId] = reach;
                        }
                    }

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

BMPReach *BMPReachFactory::ReachBMP(int reachId)
{
    map<int, BMPReach *>::iterator it = m_bmpReachs.find(reachId);
    if (it == m_bmpReachs.end()) return NULL;
    else return it->second;
}

void BMPReachFactory::Dump(ostream *fs)
{
    map<int, BMPReach *>::iterator it;
    for (it = m_bmpReachs.begin(); it != m_bmpReachs.end(); it++)
    {
        if (it->second != NULL) it->second->Dump(fs);
    }
}

void BMPReachFactory::loadTimeSeriesData(string databasePath, time_t startTime, time_t endTime, int interval)
{
    map<int, BMPReach *>::iterator it;
    for (it = m_bmpReachs.begin(); it != m_bmpReachs.end(); it++)
    {
        if (it->second != NULL) it->second->loadTimeSeriesData(databasePath, startTime, endTime, interval);
    }
}

float BMPReachFactory::getTimeSeriesData(time_t t, int reach, int dataType)
{
    map<int, BMPReach *>::iterator it = m_bmpReachs.find(reach);
    if (it == m_bmpReachs.end()) return 0.0f;
    else return it->second->getTimeSeriesData(dataType, t);
}

int BMPReachFactory::getMaxStructureId()
{
    int id = -1;
    map<int, BMPReach *>::iterator it;
    for (it = m_bmpReachs.begin(); it != m_bmpReachs.end(); it++)
    {
        if (it->second->ID() > id) id = it->second->ID();
    }

    return id;
}