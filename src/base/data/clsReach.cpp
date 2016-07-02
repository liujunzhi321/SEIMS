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
#include <iostream>
using namespace std;

clsReach::clsReach(const bson_t *&bsonTable, bson_iter_t &iterator)
{
	/// reset default values
	Reset();
	if (bson_iter_init_find(&iterator,bsonTable,REACH_SUBBASIN))
		this->Subbasin = GetIntFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_DOWNSTREAM)) 
		this->DownStream = GetIntFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_UPDOWN_ORDER)) 
		this->UpDownOrder = GetIntFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_DOWNUP_ORDER)) 
		this->DownUpOrder = GetIntFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_WIDTH)) 
		this->Width = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_LENGTH)) 
		this->Length = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_DEPTH)) 
		this->Depth = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_V0)) 
		this->V0 = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_AREA)) 
		this->Area = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_MANNING)) 
		this->Manning = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_SLOPE)) 
		this->Slope = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_GROUP)) 
		this->Group = GetIntFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_GROUPDIVIDED)) 
		this->GroupDivided = GetIntFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_NUMCELLS)) 
		this->NumCells = GetIntFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_BC1)) 
		this->bc1 = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_BC2)) 
		this->bc2 = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_BC3)) 
		this->bc3 = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_BC4)) 
		this->bc4 = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_RS1)) 
		this->rs1 = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_RS2)) 
		this->rs2 = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_RS3)) 
		this->rs3 = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_RS4)) 
		this->rs4 = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_RS5)) 
		this->rs5 = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_RK1)) 
		this->rk1 = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_RK2)) 
		this->rk2 = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_RK3)) 
		this->rk3 = GetFloatFromBSONITER(&iterator);
	if (bson_iter_init_find(&iterator,bsonTable,REACH_RK4)) 
		this->rk4 = GetFloatFromBSONITER(&iterator);
}

clsReach::~clsReach(void)
{
}

void clsReach::Reset(void)
{
	Area						= NODATA_VALUE;
	Depth					= NODATA_VALUE;
	DownStream		= -1;
	DownUpOrder		= -1;
	Group					= -1;
	GroupDivided		= -1;
	Length					= NODATA_VALUE;
	Manning				= NODATA_VALUE;
	NumCells			= -1;
	Slope					= NODATA_VALUE;
	Subbasin				= -1;
	UpDownOrder		= -1;
	V0						= NODATA_VALUE;
	Width					= NODATA_VALUE;
	bc1						= 0.55;
	bc2						= 1.1;
	bc3						= 0.21;
	bc4						= 0.35;
	rk1						= 1.71;
	rk2						= 50.;
	rk3						= 0.36;
	rk4						= 2.;
	rs1						= 1.;
	rs2						= 0.05;
	rs3						= 0.5;
	rs4						= 0.05;
	rs5 						= 0.05;
}

clsReaches::clsReaches(mongoc_client_t *conn,string& dbName, string& collectionName)
{
	bson_t		*b = bson_new();
	bson_t		*child1 = bson_new();
	bson_t		*child2 = bson_new();
	BSON_APPEND_DOCUMENT_BEGIN(b,"$query",child1);  /// query all records
	bson_append_document_end(b,child1);
	BSON_APPEND_DOCUMENT_BEGIN(b,"$orderby",child2); /// and order by subbasin ID
	BSON_APPEND_INT32(child2,REACH_SUBBASIN,1);
	bson_append_document_end(b,child2);
	bson_destroy(child1);
	bson_destroy(child2);

	mongoc_cursor_t		*cursor;
	const bson_t		*bsonTable;
	mongoc_collection_t	*collection;
	bson_error_t		*err = NULL;

	collection = mongoc_client_get_collection(conn,dbName.c_str(),collectionName.c_str());
	const bson_t		*qCount = bson_new();
	this->reachNum = (int)mongoc_collection_count(collection,MONGOC_QUERY_NONE,qCount,0,0,NULL,err);
	if(err != NULL || this->reachNum < 0)
		throw ModelException("clsReaches","ReadAllReachInfo","Failed to get document number of collection: " + collectionName + ".\n");
	cursor = mongoc_collection_find(collection,MONGOC_QUERY_NONE,0,0,0,b,NULL,NULL);

	bson_iter_t			iterator;
	while(mongoc_cursor_more(cursor) && mongoc_cursor_next(cursor,&bsonTable)) 
	{
		clsReach *curReach = new clsReach(bsonTable, iterator);
		reachesInfo[curReach->GetSubbasinID()] = curReach;
		this->reachIDs.push_back(curReach->GetSubbasinID());
	}
	vector<int>(reachIDs).swap(reachIDs);
	bson_destroy(b);
	mongoc_collection_destroy(collection);
	mongoc_cursor_destroy(cursor);
}

clsReaches::~clsReaches()
{
	if (!reachesInfo.empty())
	{
		for (map<int, clsReach*>::iterator iter = reachesInfo.begin(); iter != reachesInfo.end(); iter++)
		{
			if(iter->second != NULL)
				delete iter->second;
		}
	}
}