#include "Scenario.h"
#include "ModelException.h"
#include "utils.h"
#include <sstream>
#include <fstream>
//#include "BMPReach.h"
//#include "BMPArealNonStructuralFactory.h"

namespace MainBMP
{
Scenario::Scenario(mongoc_client_t* conn,string dbName,int scenarioID):
m_conn(conn), m_bmpDBName(dbName)
{
	if(scenarioID >= 0)
		this->m_id = scenarioID;
	else
		throw ModelException("LoadBMPsScenario", "InitiateScenario", "The scenario ID must be greater than or equal to 0.\n");
	loadScenario();
}

Scenario::~Scenario(void)
{
	map<int, BMPFactory*>::iterator it;
	for(it = this->m_bmpFactories.begin();it!=this->m_bmpFactories.end();it++)
	{
		if(it->second != NULL) delete (it->second);
		m_bmpFactories.erase(it);              /// added by Liangjun
	}
	m_bmpFactories.clear();                     /// added by Liangjun
}

void Scenario::loadScenario()
{
	GetCollectionNames(m_conn,m_bmpDBName,m_bmpCollections);
	loadScenarioName();
	loadBMPs();
	loadBMPDetail();
}

void Scenario::loadScenarioName()
{
#ifdef SQLITE
	/// previous version with SQLite, deprecated by Liangjun
	//if(!DBManager::IsTableExist(bmpPath(),TAB_BMP_SCENARIO))
	//	throw ModelException("Scenario","loadScenarioName","The BMP database '" + bmpPath() + 
	//	"' does not exist or the there is not a table named '" + TAB_BMP_SCENARIO + "' in BMP database.");

	//DBManager db;
	//db.Open(bmpPath());
	//try
	//{
	//	utils util;
	//	ostringstream str;
	//	str << "SELECT distinct NAME FROM " << TAB_BMP_SCENARIO << " where " << 
	//		"ID = " << this->m_id << " or " <<
	//		"ID = " << BASE_SCENARIO_ID << 
	//		" order by ID DESC";
	//	string strSQL = str.str();
	//
	//	slTable* tbl = db.Load(strSQL);
	//	if(tbl->nRows > 0)
	//	{
	//		this->m_name = tbl->FieldValue(1,0);
	//	}
	//	else
	//	{
	//		str << "Can't find the name of scenario " << this->m_id << ".";
	//		throw ModelException("Scenario","loadScenarioName",str.str());
	//	}

	//	delete tbl;
	//	db.Close();
	//}
	//catch(...)
	//{
	//	db.Close();
	//	throw;
	//}
#else
	vector<string>::iterator it = find(m_bmpCollections.begin(),m_bmpCollections.end(),string(TAB_BMP_SCENARIO));
	if(it == m_bmpCollections.end())
		throw ModelException("BMP Scenario","loadScenarioName","The BMP database '" + m_bmpDBName + 
		"' does not exist or there is not a table named '" + TAB_BMP_SCENARIO + "' in BMP database.");
	mongoc_collection_t *sceCollection;
	sceCollection = mongoc_client_get_collection(m_conn, m_bmpDBName.c_str(), TAB_BMP_SCENARIO);
	/// Find the unique scenario name
	bson_t		*query = bson_new(), *reply = bson_new();
	query = BCON_NEW("distinct",BCON_UTF8(TAB_BMP_SCENARIO),"key",FLD_SCENARIO_NAME, 
		"query","{",FLD_SCENARIO_ID,BCON_INT32(m_id),"}");
	bson_iter_t iter, sub_iter;
	bson_error_t *err = NULL;
	if (mongoc_collection_command_simple(sceCollection,query,NULL,reply,err))
	{
		//cout<<bson_as_json(reply,NULL)<<endl;
		if(bson_iter_init_find(&iter, reply, "values")&&
			(BSON_ITER_HOLDS_DOCUMENT (&iter) || BSON_ITER_HOLDS_ARRAY (&iter)) &&
			bson_iter_recurse (&iter, &sub_iter))
		{
			while(bson_iter_next(&sub_iter)){
				m_name = GetStringFromBSONITER(&sub_iter);
				break;
			}
		}
		else
			throw ModelException("BMP Scenario","loadScenarioName","There is not scenario existed with the ID: " + ValueToString(m_id)
			 + " in " + TAB_BMP_SCENARIO + " table in BMP database.");
	}
	bson_destroy(query);
	bson_destroy(reply);
	mongoc_collection_destroy(sceCollection);
#endif
}

void Scenario::loadBMPs()
{
#ifdef SQLITE
	if(!DBManager::IsTableExist(bmpPath(),TAB_BMP_INDEX))
		throw ModelException("Scenario","loadBMPs","The BMP database '" + bmpPath() + 
		"' does not exist or there is not a table named '" + TAB_BMP_INDEX + "' in BMP database.");

	DBManager db;
	db.Open(bmpPath());
	try
	{
		utils util;
		ostringstream str;
		str << "SELECT b.ID,b.Type,distribution,parameter,a.ID FROM " << 
			TAB_BMP_SCENARIO << " as a," << TAB_BMP_INDEX <<" as b where " << 
			"(a.ID = " << this->m_id << " or " <<
			"a.ID = " << BASE_SCENARIO_ID << " ) and (a.BMP = b.ID)" <<
			" order by a.ID DESC";

		string strSQL = str.str();
	
		slTable* tbl = db.Load(strSQL);
		if(tbl->nRows > 0)
		{
			for(int i=1;i<=tbl->nRows;i++)
			{
				int bmpId = atoi(tbl->FieldValue(i,0).c_str());
				int bmpType = atoi(tbl->FieldValue(i,1).c_str());
				string distribution = tbl->FieldValue(i,2);
				string parameter = tbl->FieldValue(i,3);
				int scenarioId = atoi(tbl->FieldValue(i,4).c_str());

				if(this->m_bmpFactories.find(bmpId) == this->m_bmpFactories.end())
				{
					if(bmpType == BMP_TYPE_REACH)
						this->m_bmpFactories[bmpId] = new BMPReachFactory(scenarioId,bmpId,bmpType,distribution,parameter);
					if(bmpType == BMP_TYPE_AREAL_NON_STRUCTURAL)
						this->m_bmpFactories[bmpId] = new BMPArealNonStructuralFactory(scenarioId,bmpId,bmpType,distribution,parameter);
				}
			}
		}

		delete tbl;
		db.Close();
	}
	catch(...)
	{
		db.Close();
		throw;
	}
#else
	vector<string>::iterator it = find(m_bmpCollections.begin(),m_bmpCollections.end(),string(TAB_BMP_INDEX));
	if(it == m_bmpCollections.end())
		throw ModelException("BMP Scenario","loadScenarioName","The BMP database '" + m_bmpDBName + 
		"' does not exist or there is not a table named '" + TAB_BMP_INDEX + "' in BMP database.");
	bson_t* query = bson_new();
	BSON_APPEND_INT32(query,FLD_SCENARIO_ID,m_id);
	//cout<<bson_as_json(query, NULL)<<endl;
	mongoc_collection_t* collection = mongoc_client_get_collection(m_conn,m_bmpDBName.c_str(),TAB_BMP_SCENARIO);
	mongoc_collection_t* collbmpidx = mongoc_client_get_collection(m_conn,m_bmpDBName.c_str(),TAB_BMP_INDEX);
	mongoc_cursor_t* cursor = mongoc_collection_find(collection,MONGOC_QUERY_NONE,0,0,0,query,NULL,NULL);
	bson_error_t *err = NULL;
	if (mongoc_cursor_error(cursor,err))
		throw ModelException("BMP Scenario", "loadBMPs","There are no record with scenario ID: " + ValueToString(m_id));
	const bson_t			*info;
	while(mongoc_cursor_more(cursor) && mongoc_cursor_next(cursor,&info)){
		bson_iter_t	iter;
		int BMPID;
		int subScenario;
		string distribution;
		string parameter;
		string location;
		if(bson_iter_init_find(&iter,info,FLD_SCENARIO_BMPID))BMPID = (int)GetFloatFromBSONITER(&iter);
		if(bson_iter_init_find(&iter,info,FLD_SCENARIO_SUB))subScenario = (int)GetFloatFromBSONITER(&iter);
		if(bson_iter_init_find(&iter,info,FLD_SCENARIO_DIST)) distribution = GetStringFromBSONITER(&iter);
		if(bson_iter_init_find(&iter,info, FLD_SCENARIO_LOOKUP)) parameter = GetStringFromBSONITER(&iter);
		if(bson_iter_init_find(&iter,info, FLD_SCENARIO_LOCATION)) location = GetStringFromBSONITER(&iter);

		int BMPType;
		int BMPPriority;
		bson_t* queryBMP = bson_new();
		BSON_APPEND_INT32(queryBMP,FLD_BMP_ID,BMPID);
		mongoc_cursor_t *cursor2 = mongoc_collection_find(collbmpidx,MONGOC_QUERY_NONE,0,0,0,queryBMP,NULL,NULL);
		if (mongoc_cursor_error(cursor2,err))
			throw ModelException("BMP Scenario", "loadBMPs","There are no record with BMP ID: " + ValueToString(BMPID));
		const bson_t *info2;
		while(mongoc_cursor_more(cursor2) && mongoc_cursor_next(cursor2,&info2)){
			bson_iter_t	sub_iter;
			if(bson_iter_init_find(&sub_iter,info2,FLD_BMP_TYPE))BMPType = (int)GetFloatFromBSONITER(&sub_iter);
			if(bson_iter_init_find(&sub_iter,info2,FLD_BMP_PRIORITY))BMPPriority = (int)GetFloatFromBSONITER(&sub_iter);
		}
		//cout<<BMPID<<","<<BMPType<<","<<distribution<<","<<parameter<<endl;
		bson_destroy(queryBMP);
		mongoc_cursor_destroy(cursor2);
		/// Combine BMPID, and SubScenario for a unique ID to identify "different" BMP
		int uniqueBMPID = BMPID * 100 + subScenario;
		if(this->m_bmpFactories.find(uniqueBMPID) == this->m_bmpFactories.end())
		{
			if (BMPID == BMP_TYPE_PLANT_MGT)
			{
				this->m_bmpFactories[uniqueBMPID] = new BMPPlantMgtFactory(m_id,BMPID,subScenario,BMPType,BMPPriority,distribution,parameter,location);
			}
			/*if(BMPType == BMP_TYPE_REACH)
			this->m_bmpFactories[BMPID] = new BMPReachFactory(m_id,BMPID,BMPType,distribution,parameter);
			if(BMPType == BMP_TYPE_AREAL_NON_STRUCTURAL)
			this->m_bmpFactories[BMPID] = new BMPArealNonStructuralFactory(m_id,BMPID,BMPType,distribution,parameter);*/
		}
	}
	bson_destroy(query);
	mongoc_cursor_destroy(cursor);
	mongoc_collection_destroy(collection);
	mongoc_collection_destroy(collbmpidx);
#endif
}

void Scenario::loadBMPDetail()
{
	map<int,BMPFactory*>::iterator it;
	for(it = this->m_bmpFactories.begin();it!=this->m_bmpFactories.end();it++)
	{
		it->second->loadBMP(this->m_conn, m_bmpDBName);
	}
}

void Scenario::Dump(string fileName)
{
	ofstream fs;
	utils util;
	fs.open(fileName.c_str(), ios::out);
	if (fs.is_open())
	{
		Dump(&fs);
		fs.close();
	}
}

void Scenario::Dump(ostream* fs)
{
	if(fs == NULL) return;

	*fs << "Scenario" << endl;
	*fs << "ID:" << this->m_id << endl;
	*fs << "Name:" << this->m_name << endl;
	*fs << "*** All the BMPs ***" << endl;
	map<int,BMPFactory*>::iterator it;
	for(it = this->m_bmpFactories.begin();it!=this->m_bmpFactories.end();it++)
	{
		if(it->second != NULL) it->second->Dump(fs);
	}
}
}
//void Scenario::loadTimeSeriesData(string databasePath, time_t startTime, time_t endTime,int interval)
//{
//	map<int,BMPFactory*>::iterator it;
//	for(it = this->m_bmpFactories.begin();it!=this->m_bmpFactories.end();it++)
//	{
//		if(it->second == NULL) continue;
//
//		if(it->second->bmpType() != BMP_TYPE_REACH) continue;
//
//		BMPReachFactory* reach = (BMPReachFactory*)(it->second);
//
//		reach->loadTimeSeriesData(databasePath,startTime,endTime,interval);
//	}
//}
//
//BMPReach* Scenario::getReachStructure(int reach,int reachBMPid)
//{
//	map<int,BMPFactory*>::iterator it;
//
//	it = this->m_bmpFactories.find(reachBMPid);
//	if(it == this->m_bmpFactories.end()) return NULL;
//	else
//	{
//		BMPReachFactory* reachF = (BMPReachFactory* )(it->second);
//		return reachF->ReachBMP(reach);
//	}
//}
//
//BMPReachPointSource* Scenario::getPointSource(int reach)
//{
//	BMPReach* r= getReachStructure(reach,BMP_TYPE_POINTSOURCE);
//	if(r==NULL) return NULL;
//	else return (BMPReachPointSource*)r;
//}
//
//BMPReachFlowDiversion* Scenario::getFlowDiversion(int reach)
//{
//	BMPReach* r=  getReachStructure(reach,BMP_TYPE_FLOWDIVERSION_STREAM);
//	if(r==NULL) return NULL;
//	else return (BMPReachFlowDiversion*)r;
//}
//
//BMPReachReservoir* Scenario::getReservoir(int reach)
//{
//	BMPReach* r=  getReachStructure(reach,BMP_TYPE_RESERVOIR);
//	if(r==NULL) return NULL;
//	else return (BMPReachReservoir*)r;
//}
//
//int Scenario::getMaxReservoirId()
//{
//	map<int,BMPFactory*>::iterator it;
//
//	it = this->m_bmpFactories.find(BMP_TYPE_RESERVOIR);
//	if(it == this->m_bmpFactories.end()) return 0;
//	else
//	{
//		BMPReachFactory* reachF = (BMPReachFactory* )(it->second);
//		return reachF->getMaxStructureId();
//	}
//}
//
//NonStructural::ManagementOperation* Scenario::getOperation(
//	int validCellIndex,int startYear,time_t currentTime,int BMPid,bool useSecond)
//{
//	map<int,BMPFactory*>::iterator it;
//
//	it = this->m_bmpFactories.find(BMPid);
//	if(it == this->m_bmpFactories.end()) return NULL;
//	else
//	{
//		BMPArealNonStructural* arealS = (BMPArealNonStructural* )(it->second);
//		if(useSecond)	return arealS->getSecondOperation(validCellIndex,startYear,currentTime);
//		else	
//			return arealS->getOperation(validCellIndex,startYear,currentTime);
//	}
//}

//}

/// Deprecated by Liangjun
//string Scenario::bmpPath()
//{
//	return this->m_projectPath + BMP_DATABASE_NAME;
//}