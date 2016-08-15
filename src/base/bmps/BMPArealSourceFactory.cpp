/*!
 * \brief Implementation of BMPArealSrcFactory class.
 * \author Liang-Jun Zhu
 * \date July 2016
 *
 */
#include "BMPArealSourceFactory.h"
#include "utils.h"

using namespace MainBMP;

BMPArealSrcFactory::BMPArealSrcFactory(int scenarioId, int bmpId, int subScenario, int bmpType, int bmpPriority,
                                       string distribution, string collection, string location)
        : BMPFactory(scenarioId, bmpId, subScenario, bmpType, bmpPriority, distribution, collection, location)
{
    m_arealSrcMgtTab = m_bmpCollection;
    m_arealSrcIDs = utils::SplitStringForInt(location, ',');
    vector<string> dist = utils::SplitString(distribution, '|');
    if (dist.size() == 3 && StringMatch(dist[0], FLD_SCENARIO_DIST_ARRAY))
    {
        m_arealSrcDistName = dist[1];
        m_arealSrc = atoi(dist[2].c_str());
    }
    else
        throw ModelException("BMPPointSourceFactory", "Initialization",
                             "The distribution field must follow the format: ARRAY|CollectionName|PTSRC.\n");
}

BMPArealSrcFactory::~BMPArealSrcFactory(void)
{
    if (!m_arealSrcLocsMap.empty())
    {
        for (map<int, ArealBMPLocations *>::iterator it = m_arealSrcLocsMap.begin();
             it != m_arealSrcLocsMap.end();)
        {
            if (it->second != NULL)
            {
                delete it->second;
                it->second = NULL;
            }
            it = m_arealSrcLocsMap.erase(it);
        }
        m_arealSrcLocsMap.clear();
    }
    if (!m_pointSrcMgtMap.empty())
    {
        for (map<int, ArealSourceMgtParams *>::iterator it = m_pointSrcMgtMap.begin();
             it != m_pointSrcMgtMap.end();)
        {
            if (it->second != NULL)
            {
                delete it->second;
                it->second = NULL;
            }
            it = m_pointSrcMgtMap.erase(it);
        }
        m_pointSrcMgtMap.clear();
    }
}

void BMPArealSrcFactory::loadBMP(mongoc_client_t *conn, string &bmpDBName)
{
    ReadArealSourceManagements(conn, bmpDBName);
    ReadArealSourceLocations(conn, bmpDBName);
}

void BMPArealSrcFactory::ReadArealSourceManagements(mongoc_client_t *conn, string &bmpDBName)
{
    bson_t *b = bson_new();
    bson_t *child1 = bson_new(), *child2 = bson_new();
    BSON_APPEND_DOCUMENT_BEGIN(b, "$query", child1);
    BSON_APPEND_INT32(child1, BMP_FLD_SUB, m_subScenarioId);
    bson_append_document_end(b, child1);
    BSON_APPEND_DOCUMENT_BEGIN(b, "$orderby", child2);
    BSON_APPEND_INT32(child2, BMP_FLD_SEQUENCE, 1);
    bson_append_document_end(b, child2);
    bson_destroy(child1);
    bson_destroy(child2);

    mongoc_cursor_t *cursor;
    const bson_t *bsonTable;
    mongoc_collection_t *collection = NULL;
    bson_iter_t iter;

    collection = mongoc_client_get_collection(conn, bmpDBName.c_str(), m_arealSrcMgtTab.c_str());
    if (collection == NULL)
        throw ModelException("BMPs Scenario", "Read Point Source Management",
                             "Failed to get collection: " + m_arealSrcMgtTab + ".\n");
    cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE, 0, 0, 0, b, NULL, NULL);

    int count = 1; /// Use count to counting sequence number, in case of discontinuous or repeat of SEQUENCE in database.
    while (mongoc_cursor_next(cursor, &bsonTable))
    {
        m_arealSrcMgtSeqs.push_back(count);
        m_pointSrcMgtMap[count] = new ArealSourceMgtParams(bsonTable, iter);
        count++;
    }
    bson_destroy(b);
    mongoc_collection_destroy(collection);
    mongoc_cursor_destroy(cursor);
}

void BMPArealSrcFactory::ReadArealSourceLocations(mongoc_client_t *conn, string &bmpDBName)
{
    bson_t *b = bson_new();
    bson_t *child1 = bson_new();
    BSON_APPEND_DOCUMENT_BEGIN(b, "$query", child1);
    BSON_APPEND_INT32(child1, BMP_PTSRC_FLD_CODE, m_arealSrc);
    bson_append_document_end(b, child1);
    bson_destroy(child1);

    mongoc_cursor_t *cursor;
    const bson_t *bsonTable;
    mongoc_collection_t *collection = NULL;
    bson_iter_t iter;

    collection = mongoc_client_get_collection(conn, bmpDBName.c_str(), m_arealSrcDistName.c_str());
    if (collection == NULL)
        throw ModelException("BMPs Scenario", "Read Point Source Locations",
                             "Failed to get collection: " + m_arealSrcDistName + ".\n");
    cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE, 0, 0, 0, b, NULL, NULL);

    
    bson_destroy(b);
    mongoc_collection_destroy(collection);
    mongoc_cursor_destroy(cursor);
}

void BMPArealSrcFactory::Dump(ostream *fs)
{
    if (fs == NULL) return;
    *fs << "Point Source Management Factory: " << endl <<
    "    SubScenario ID: " << m_subScenarioId << " PTSRC: " << m_arealSrc << endl;
    for (vector<int>::iterator it = m_arealSrcMgtSeqs.begin(); it != m_arealSrcMgtSeqs.end(); it++)
    {
        map<int, ArealSourceMgtParams *>::iterator findIdx = m_pointSrcMgtMap.find(*it);
        if (findIdx != m_pointSrcMgtMap.end())
            m_pointSrcMgtMap[*it]->Dump(fs);
    }
    for (vector<int>::iterator it = m_arealSrcIDs.begin(); it != m_arealSrcIDs.end(); it++)
    {
        map<int, ArealBMPLocations *>::iterator findIdx = m_arealSrcLocsMap.find(*it);
        if (findIdx != m_arealSrcLocsMap.end())
            m_arealSrcLocsMap[*it]->Dump(fs);
    }
}


/************************************************************************/
/*                  ArealSourceMgtParams                                        */
/************************************************************************/

ArealSourceMgtParams::ArealSourceMgtParams(const bson_t *&bsonTable, bson_iter_t &iter)
        : m_startDate(0), m_endDate(0), m_waterVolume(0.f), m_sedimentConc(0.f), m_TNConc(0.f), m_NO3Conc(0.f),
          m_NH3Conc(0.f), m_OrgNConc(0.f), m_TPConc(0.f), m_MinPConc(0.f), m_OrgPConc(0.f), m_name(""), m_seqence(-1)
{
    if (bson_iter_init_find(&iter, bsonTable, BMP_FLD_NAME))
        m_name = GetStringFromBSONITER(&iter);
    if (bson_iter_init_find(&iter, bsonTable, BMP_FLD_SEQUENCE))
        m_seqence = GetIntFromBSONITER(&iter);
    if (bson_iter_init_find(&iter, bsonTable, BMP_PTSRC_FLD_Q))
        m_waterVolume = GetFloatFromBSONITER(&iter);
    if (bson_iter_init_find(&iter, bsonTable, BMP_PTSRC_FLD_SED))
        m_sedimentConc = GetFloatFromBSONITER(&iter);
    if (bson_iter_init_find(&iter, bsonTable, BMP_PTSRC_FLD_TN))
        m_TNConc = GetFloatFromBSONITER(&iter);
    if (bson_iter_init_find(&iter, bsonTable, BMP_PTSRC_FLD_NO3))
        m_NO3Conc = GetFloatFromBSONITER(&iter);
    if (bson_iter_init_find(&iter, bsonTable, BMP_PTSRC_FLD_NH3))
        m_NH3Conc = GetFloatFromBSONITER(&iter);
    if (bson_iter_init_find(&iter, bsonTable, BMP_PTSRC_FLD_ORGN))
        m_OrgNConc = GetFloatFromBSONITER(&iter);
    if (bson_iter_init_find(&iter, bsonTable, BMP_PTSRC_FLD_TP))
        m_TPConc = GetFloatFromBSONITER(&iter);
    if (bson_iter_init_find(&iter, bsonTable, BMP_PTSRC_FLD_MINP))
        m_MinPConc = GetFloatFromBSONITER(&iter);
    if (bson_iter_init_find(&iter, bsonTable, BMP_PTSRC_FLD_ORGP))
        m_OrgPConc = GetFloatFromBSONITER(&iter);
    int sYear, sMonth, sDay, eYear, eMonth, eDay;
    if (bson_iter_init_find(&iter, bsonTable, BMP_FLD_SYEAR))
        sYear = GetIntFromBSONITER(&iter);
    if (bson_iter_init_find(&iter, bsonTable, BMP_FLD_SMONTH))
        sMonth = GetIntFromBSONITER(&iter);
    if (bson_iter_init_find(&iter, bsonTable, BMP_FLD_SDAY))
        sDay = GetIntFromBSONITER(&iter);
    if (bson_iter_init_find(&iter, bsonTable, BMP_FLD_EYEAR))
        eYear = GetIntFromBSONITER(&iter);
    if (bson_iter_init_find(&iter, bsonTable, BMP_FLD_EMONTH))
        eMonth = GetIntFromBSONITER(&iter);
    if (bson_iter_init_find(&iter, bsonTable, BMP_FLD_EDAY))
        eDay = GetIntFromBSONITER(&iter);
    if (sYear > 0 && sMonth > 0 && sDay > 0)
        m_startDate = utils::ConvertYMDToTime(sYear, sMonth, sDay);
    if (eYear > 0 && eMonth > 0 && eDay > 0)
        m_endDate = utils::ConvertYMDToTime(eYear, eMonth, eDay);
}

ArealSourceMgtParams::~ArealSourceMgtParams(void)
{
}

void ArealSourceMgtParams::Dump(ostream *fs)
{
    if (fs == NULL) return;
    *fs << "    Point Source Managements: " << endl;
    if (m_startDate != 0)
        *fs << "      Start Date: " << utils::ConvertToString(&m_startDate) << endl;
    if (m_endDate != 0)
        *fs << "      End Date: " << utils::ConvertToString(&m_endDate) << endl;
    *fs << "      WaterVolume: " << m_waterVolume << ", Sediment: " << m_sedimentConc <<
    ", TN: " << m_TNConc << ", NO3: " << m_NO3Conc <<
    ", NH3: " << m_NH3Conc << ", OrgN: " << m_OrgNConc <<
    ", TP: " << m_TPConc << ", MinP: " << m_MinPConc <<
    ", OrgP: " << m_OrgPConc << endl;
}


/************************************************************************/
/*                      ArealBMPLocations                                           */
/************************************************************************/

ArealBMPLocations::ArealBMPLocations(vector<int>& locations, int luccID, clsRasterData* fieldRaster, clsRasterData* luccRaster)
{
    
}

ArealBMPLocations::~ArealBMPLocations(void)
{
}

void ArealBMPLocations::Dump(ostream *fs)
{
    if (fs == NULL) return;
    *fs << "      Point Source Location: " << endl <<
    "        PTSRCID: " << m_arealSrcID << ", Valid Cells Number: " << m_nCells <<
    ", Size: " << m_size << endl;
}