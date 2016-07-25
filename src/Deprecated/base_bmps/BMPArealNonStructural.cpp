#include "BMPArealNonStructural.h"
#include "utils.h"
#include "DBManager.h"
#include "ManagementOperationPlant.h"
#include "ManagementOperationHarvest.h"
#include "ManagementOperationFertilizer.h"
#include "ManagementOperationTillage.h"
#include "CropParameter.h"
#include "TillageParameter.h"
#include "FertilizerParameter.h"

using namespace MainBMP;

BMPArealNonStructural::BMPArealNonStructural(string bmpDatabasePath,
                                             string parameterTableName,
                                             int bmpId,
                                             string bmpName,
                                             string distribution,
                                             int scenario) : BMPBase(bmpDatabasePath, bmpId, bmpName,
                                                                     BMP_TYPE_AREAL_NON_STRUCTURAL, parameterTableName)
{
    m_maxYear = 0;
    m_scenarioId = scenario;

    utils util;
    vector<string> distributions = util.SplitString(distribution, '/');
    m_distributionMapName = distributions[0];
    m_distributionTableName = distributions[1];

    m_currentScearioDistribution = new clsRasterData(bmpDatabasePath + m_distributionMapName);
    loadOperations();
}

string BMPArealNonStructural::getAllQueryColumns()
{
    if (m_bmpId == BMP_TYPE_CROP) return "CropCode,PlantingMon,PlantingDay,HarvestMon,HarvestDay";
    else if (m_bmpId == BMP_TYPE_FERTILIZER) return "FerMon,FerDay,FerType,FerRate";
    else if (m_bmpId == BMP_TYPE_TILLAGE) return "TillMon,TillDay,TillCode";
    else return "";
}

void BMPArealNonStructural::loadParameters()
{
    if (m_bmpType != BMP_TYPE_AREAL_NON_STRUCTURAL)
        throw ModelException("BMPArealNonStructural", "loadOperations", "Error BMP type for areal not-structural BMP!");

    //read all the reaches in reach_BMP table
    string bmpDatabase = m_bmpDatabasePath + BMP_DATABASE_NAME;
    if (!DBManager::IsTableExist(bmpDatabase, m_parameterTableName))
        throw ModelException("BMPArealNonStructural", "loadOperations", "The BMP database '" + bmpDatabase +
                                                                        "' does not exist or the there is not a table named '" +
                                                                        m_parameterTableName + "' in BMP database.");

    DBManager db;
    db.Open(bmpDatabase);
    try
    {
        string strSQL = "select * from " + m_parameterTableName;
        slTable *tbl = db.Load(strSQL);

        if (tbl->nRows > 0)
        {
            for (int i = 1; i <= tbl->nRows; i++)
            {
                vector<string> oneRow;
                for (int j = 0; j < tbl->nCols; j++)
                {
                    oneRow.push_back(tbl->FieldValue(i, j));
                }

                BMPParameter::ArealParameter *p = NULL;
                if (m_bmpId == BMP_TYPE_CROP)
                    p = (BMPParameter::ArealParameter *) (new BMPParameter::CropParameter(&oneRow));
                else if (m_bmpId == BMP_TYPE_FERTILIZER)
                    p = (BMPParameter::ArealParameter *) (new BMPParameter::FertilizerParameter(&oneRow));
                else if (m_bmpId == BMP_TYPE_TILLAGE)
                    p = (BMPParameter::ArealParameter *) (new BMPParameter::TillageParameter(&oneRow));

                if (p != NULL) m_operationParameters[p->ID()] = p;
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

BMPParameter::ArealParameter *BMPArealNonStructural::getParameter(int id)
{
    map<int, BMPParameter::ArealParameter *>::iterator it2 = m_operationParameters.find(id);
    if (it2 == m_operationParameters.end()) return NULL;
    return it2->second;
}

void BMPArealNonStructural::loadOperations()
{
    loadParameters();

    if (m_bmpType != BMP_TYPE_AREAL_NON_STRUCTURAL)
        throw ModelException("BMPArealNonStructural", "loadOperations", "Error BMP type for areal not-structural BMP!");

    //read all the reaches in reach_BMP table
    string bmpDatabase = m_bmpDatabasePath + BMP_DATABASE_NAME;
    if (!DBManager::IsTableExist(bmpDatabase, m_distributionTableName))
        throw ModelException("BMPArealNonStructural", "loadOperations", "The BMP database '" + bmpDatabase +
                                                                        "' does not exist or the there is not a table named '" +
                                                                        m_distributionTableName + "' in BMP database.");


    DBManager db;
    db.Open(bmpDatabase);
    try
    {
        utils util;
        ostringstream str;
        str << "SELECT scenario,location," << getAllQueryColumns()
        << " FROM " << m_distributionTableName
        << " where "
        << "scenario = " << m_scenarioId << " or "
        << "scenario = " << BASE_SCENARIO_ID
        << " order by scenario DESC";

        string strSQL = str.str();
        slTable *tbl = db.Load(strSQL);
        if (tbl->nRows > 0)
        {
            map<int, int> m_locationScenarios;    //store all the location to avoid repeated location
            for (int i = 1; i <= tbl->nRows; i++)
            {
                int scenario = atoi(tbl->FieldValue(i, 0).c_str());
                int location = atoi(tbl->FieldValue(i, 1).c_str());
                map<int, int>::iterator it = m_locationScenarios.find(location);
                if (it == m_locationScenarios.end() ||    //does not exist
                    it->second == scenario)                //exists and has same scenario
                {
                    if (it == m_locationScenarios.end()) m_locationScenarios[location] = scenario;

                    int year = atoi(tbl->FieldValue(i, 2).c_str());
                    m_maxYear = max(m_maxYear, year); //get max year

                    //read data for different areal non-structural BMP
                    if (m_bmpId == BMP_TYPE_CROP) //crop management, split to plant and harvest operation
                    {
                        //plant operation
                        int cropType = atoi(tbl->FieldValue(i, 3).c_str());
                        int month = atoi(tbl->FieldValue(i, 4).c_str());
                        int day = atoi(tbl->FieldValue(i, 5).c_str());
                        NonStructural::ManagementOperationPlant *plantOp =
                                new NonStructural::ManagementOperationPlant(location, year, month, day,
                                                                            getParameter(cropType), cropType);

                        m_operations[plantOp->UniqueID()] = plantOp;

                        //harvest operation
                        month = atoi(tbl->FieldValue(i, 6).c_str());
                        day = atoi(tbl->FieldValue(i, 7).c_str());

                        NonStructural::ManagementOperationHarvest *harvestOp =
                                new NonStructural::ManagementOperationHarvest(location, year, month, day, NULL);

                        m_secondOperations[harvestOp->UniqueID()] = harvestOp;
                    }
                    else if (m_bmpId == BMP_TYPE_FERTILIZER) //fertilizer management
                    {
                        int month = atoi(tbl->FieldValue(i, 3).c_str());
                        int day = atoi(tbl->FieldValue(i, 4).c_str());
                        int ferType = atoi(tbl->FieldValue(i, 5).c_str());
                        int ferRate = atoi(tbl->FieldValue(i, 6).c_str());
                        NonStructural::ManagementOperationFertilizer *ferOp =
                                new NonStructural::ManagementOperationFertilizer(location, year, month, day,
                                                                                 getParameter(ferType), ferType,
                                                                                 ferRate);

                        m_operations[ferOp->UniqueID()] = ferOp;
                    }
                    else if (m_bmpId == BMP_TYPE_TILLAGE) //tillage management
                    {
                        int month = atoi(tbl->FieldValue(i, 3).c_str());
                        int day = atoi(tbl->FieldValue(i, 4).c_str());
                        int tillCode = atoi(tbl->FieldValue(i, 5).c_str());
                        NonStructural::ManagementOperationTillage *tillOp =
                                new NonStructural::ManagementOperationTillage(location, year, month, day,
                                                                              getParameter(tillCode), tillCode);

                        m_operations[tillOp->UniqueID()] = tillOp;
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

BMPArealNonStructural::~BMPArealNonStructural(void)
{
    //delete distribution raster data
    if (m_currentScearioDistribution != NULL) delete m_currentScearioDistribution;

    //delete all the operations
    map<string, NonStructural::ManagementOperation *>::iterator it;
    for (it = m_operations.begin(); it != m_operations.end(); it++)
    {
        if (it->second != NULL) delete it->second;
    }
    for (it = m_secondOperations.begin(); it != m_secondOperations.end(); it++)
    {
        if (it->second != NULL) delete it->second;
    }

    //delete all the parameters
    map<int, BMPParameter::ArealParameter *>::iterator it2;
    for (it2 = m_operationParameters.begin(); it2 != m_operationParameters.end(); it2++)
    {
        if (it2->second != NULL) delete it2->second;
    }
}

NonStructural::ManagementOperation *BMPArealNonStructural::getSecondOperation(int validCellIndex, int startYear,
                                                                              time_t currentTime)
{
    //get location id from raster file
    int location = int(m_currentScearioDistribution->getValue(validCellIndex));

    //get year,month,day
    struct tm dateInfo;
#ifndef linux
    localtime_s(&dateInfo, &currentTime);
#else
    localtime_r(&currentTime, &dateInfo);
#endif

    int year = 1900 + dateInfo.tm_year;
    int month = dateInfo.tm_mon + 1;
    int day = dateInfo.tm_mday;

    //time cycling
    //find the year for current time
    year = year - startYear;
    if (m_maxYear > 0) year = year % m_maxYear + 1;

    //try to find the operation for current time
    string id = NonStructural::ManagementOperation::ManagementOperation2ID(location, year, month, day);
    map<string, NonStructural::ManagementOperation *>::iterator it = m_secondOperations.find(id);
    if (it == m_secondOperations.end()) return NULL;
    else return it->second;
}

NonStructural::ManagementOperation *BMPArealNonStructural::getOperation(int validCellIndex, int startYear,
                                                                        time_t currentTime)
{
    //get location id from raster file
    int location = int(m_currentScearioDistribution->getValue(validCellIndex));

    //get year,month,day
    struct tm dateInfo;
#ifndef linux
    localtime_s(&dateInfo, &currentTime);
#else
    localtime_r(&currentTime, &dateInfo);
#endif

    int year = 1900 + dateInfo.tm_year;
    int month = dateInfo.tm_mon + 1;
    int day = dateInfo.tm_mday;

    //time cycling
    //find the year for current time
    year = year - startYear;
    if (m_maxYear > 0) year = year % m_maxYear + 1;

    //try to find the operation for current time
    string id = NonStructural::ManagementOperation::ManagementOperation2ID(location, year, month, day);
    map<string, NonStructural::ManagementOperation *>::iterator it = m_operations.find(id);
    if (it == m_operations.end()) return NULL;
    else return it->second;
}

void BMPArealNonStructural::Dump(ostream *fs)
{
    if (fs == NULL) return;

    BMPBase::Dump(fs);

    *fs << "*** All the parameters ***" << endl;
    map<int, BMPParameter::ArealParameter *>::iterator it2;
    for (it2 = m_operationParameters.begin(); it2 != m_operationParameters.end(); it2++)
    {
        if (it2->second != NULL) it2->second->Dump(fs);
    }

    *fs << "*** All the operations ***" << endl;
    *fs << "*** first ***" << endl;
    map<string, NonStructural::ManagementOperation *>::iterator it;
    for (it = m_operations.begin(); it != m_operations.end(); it++)
    {
        if (it->second != NULL) it->second->Dump(fs);
    }
    *fs << "*** second ***" << endl;
    for (it = m_secondOperations.begin(); it != m_secondOperations.end(); it++)
    {
        if (it->second != NULL) it->second->Dump(fs);
    }
}
