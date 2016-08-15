/*!
 * \file clsReach.cpp
 * \brief Class to store reach related parameters from REACHES table
 *
 * \author LiangJun Zhu
 * \version 1.0
 * \date May. 2016
 *
 */
#include "clsReach.h"
#include "util.h"
//#include <iostream>

using namespace std;

clsReach::clsReach(const bson_t *&bsonTable)
{
    bson_iter_t iterator;
    /// reset default values
    Reset();
    if (bson_iter_init_find(&iterator, bsonTable, REACH_SUBBASIN))
        this->Subbasin = GetIntFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_DOWNSTREAM))
        this->DownStream = GetIntFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_UPDOWN_ORDER))
        this->UpDownOrder = GetIntFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_DOWNUP_ORDER))
        this->DownUpOrder = GetIntFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_WIDTH))
        this->Width = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_LENGTH))
        this->Length = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_DEPTH))
        this->Depth = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_V0))
        this->V0 = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_AREA))
        this->Area = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_MANNING))
        this->Manning = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_SLOPE))
        this->Slope = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_GROUP))
        this->Group = GetIntFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_GROUPDIVIDED))
        this->GroupDivided = GetIntFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_NUMCELLS))
        this->NumCells = GetIntFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_BC1))
        this->bc1 = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_BC2))
        this->bc2 = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_BC3))
        this->bc3 = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_BC4))
        this->bc4 = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_RS1))
        this->rs1 = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_RS2))
        this->rs2 = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_RS3))
        this->rs3 = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_RS4))
        this->rs4 = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_RS5))
        this->rs5 = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_RK1))
        this->rk1 = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_RK2))
        this->rk2 = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_RK3))
        this->rk3 = GetFloatFromBSONITER(&iterator);
    if (bson_iter_init_find(&iterator, bsonTable, REACH_RK4))
        this->rk4 = GetFloatFromBSONITER(&iterator);
}

clsReach::~clsReach(void)
{
}

void clsReach::Reset(void)
{
    Area = NODATA_VALUE;
    Depth = NODATA_VALUE;
    DownStream = -1;
    DownUpOrder = -1;
    Group = -1;
    GroupDivided = -1;
    Length = NODATA_VALUE;
    Manning = NODATA_VALUE;
    NumCells = -1;
    Slope = NODATA_VALUE;
    Subbasin = -1;
    UpDownOrder = -1;
    V0 = NODATA_VALUE;
    Width = NODATA_VALUE;
    bc1 = 0.55f;
    bc2 = 1.1f;
    bc3 = 0.21f;
    bc4 = 0.35f;
    rk1 = 1.71f;
    rk2 = 50.f;
    rk3 = 0.36f;
    rk4 = 2.f;
    rs1 = 1.f;
    rs2 = 0.05f;
    rs3 = 0.5f;
    rs4 = 0.05f;
    rs5 = 0.05f;
}

clsReaches::clsReaches(mongoc_client_t *conn, string &dbName, string collectionName)
{
    bson_t *b = bson_new();
    bson_t *child1 = bson_new();
    bson_t *child2 = bson_new();
    BSON_APPEND_DOCUMENT_BEGIN(b, "$query", child1);  /// query all records
    bson_append_document_end(b, child1);
    BSON_APPEND_DOCUMENT_BEGIN(b, "$orderby", child2); /// and order by subbasin ID
    BSON_APPEND_INT32(child2, REACH_SUBBASIN, 1);
    bson_append_document_end(b, child2);
    bson_destroy(child1);
    bson_destroy(child2);

    mongoc_cursor_t *cursor;
    const bson_t *bsonTable;
    mongoc_collection_t *collection = NULL;
    bson_error_t *err = NULL;

    collection = mongoc_client_get_collection(conn, dbName.c_str(), collectionName.c_str());
    if (collection == NULL)
        throw ModelException("clsReaches", "ReadAllReachInfo", "Failed to get collection: " + collectionName + ".\n");
    const bson_t *qCount = bson_new();
    this->m_reachNum = (int) mongoc_collection_count(collection, MONGOC_QUERY_NONE, qCount, 0, 0, NULL, err);
    if (err != NULL || this->m_reachNum < 0)
        throw ModelException("clsReaches", "ReadAllReachInfo",
                             "Failed to get document number of collection: " + collectionName + ".\n");
    cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE, 0, 0, 0, b, NULL, NULL);


    while (mongoc_cursor_more(cursor) && mongoc_cursor_next(cursor, &bsonTable))
    {
        clsReach *curReach = new clsReach(bsonTable);
        m_reachesInfo[curReach->GetSubbasinID()] = curReach;
        this->m_reachIDs.push_back(curReach->GetSubbasinID());
    }
    vector<int>(m_reachIDs).swap(m_reachIDs);

    bson_destroy(b);
    mongoc_collection_destroy(collection);
    mongoc_cursor_destroy(cursor);
}

clsReaches::~clsReaches()
{
    if (!m_reachesInfo.empty())
    {
        for (map<int, clsReach *>::iterator iter = m_reachesInfo.begin(); iter != m_reachesInfo.end();)
        {
            if (iter->second != NULL)
                delete iter->second;
			iter = m_reachesInfo.erase(iter);
        }
		m_reachesInfo.clear();
    }
}