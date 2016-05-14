#include "Scenario.h"
#include "ModelException.h"
#include "utils.h"
#include <sstream>
#include "BMPReach.h"
#include "BMPArealNonStructuralFactory.h"
#include <fstream>

namespace MainBMP
{

	Scenario::Scenario(string projectPath,int scenarioID)
{
	this->m_projectPath = projectPath;
	loadScenario(scenarioID);
}

int Scenario::ID()
{
	return this->m_id;
}
bool Scenario::IsBaseScenario()
{
	return this->m_id == 0;
}
string Scenario::Name()
{
	return this->m_name;
}

Scenario::~Scenario(void)
{
	map<int,BMPFactory*>::iterator it;
	for(it = this->m_bmpFactories.begin();it!=this->m_bmpFactories.end();it++)
	{
		if(it->second != NULL) delete (it->second);
	}
}

string Scenario::bmpPath()
{
	return this->m_projectPath + BMP_DATABASE_NAME;
}

void Scenario::loadScenarioName()
{
	if(!DBManager::IsTableExist(bmpPath(),TABLE_SCENARIO))
		throw ModelException("Scenario","loadScenarioName","The BMP database '" + bmpPath() + 
		"' does not exist or the there is not a table named '" + TABLE_SCENARIO + "' in BMP database.");

	DBManager db;
	db.Open(bmpPath());
	try
	{
		utils util;
		ostringstream str;
		str << "SELECT distinct NAME FROM " << TABLE_SCENARIO << " where " << 
			"ID = " << this->m_id << " or " <<
			"ID = " << BASE_SCENARIO_ID << 
			" order by ID DESC";
		string strSQL = str.str();
	
		slTable* tbl = db.Load(strSQL);
		if(tbl->nRows > 0)
		{
			this->m_name = tbl->FieldValue(1,0);
		}
		else
		{
			str << "Can't find the name of scenario " << this->m_id << ".";
			throw ModelException("Scenario","loadScenarioName",str.str());
		}

		delete tbl;
		db.Close();
	}
	catch(...)
	{
		db.Close();
		throw;
	}
}

void Scenario::loadBMPs()
{
	if(!DBManager::IsTableExist(bmpPath(),TABLE_INDEX))
		throw ModelException("Scenario","loadBMPs","The BMP database '" + bmpPath() + 
		"' does not exist or there is not a table named '" + TABLE_INDEX + "' in BMP database.");

	DBManager db;
	db.Open(bmpPath());
	try
	{
		utils util;
		ostringstream str;
		str << "SELECT b.ID,b.Type,distribution,parameter,a.ID FROM " << 
			TABLE_SCENARIO << " as a," << TABLE_INDEX <<" as b where " << 
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
}

void Scenario::loadBMPDetail()
{
	map<int,BMPFactory*>::iterator it;
	for(it = this->m_bmpFactories.begin();it!=this->m_bmpFactories.end();it++)
	{
		it->second->loadBMP(this->m_projectPath);
	}
}

void Scenario::loadScenario(int scenarioID)
{
	this->m_id = scenarioID;
	loadScenarioName();
	loadBMPs();
	loadBMPDetail();
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

void Scenario::loadTimeSeriesData(string databasePath, time_t startTime, time_t endTime,int interval)
{
	map<int,BMPFactory*>::iterator it;
	for(it = this->m_bmpFactories.begin();it!=this->m_bmpFactories.end();it++)
	{
		if(it->second == NULL) continue;

		if(it->second->bmpType() != BMP_TYPE_REACH) continue;

		BMPReachFactory* reach = (BMPReachFactory*)(it->second);

		reach->loadTimeSeriesData(databasePath,startTime,endTime,interval);
	}
}

BMPReach* Scenario::getReachStructure(int reach,int reachBMPid)
{
	map<int,BMPFactory*>::iterator it;

	it = this->m_bmpFactories.find(reachBMPid);
	if(it == this->m_bmpFactories.end()) return NULL;
	else
	{
		BMPReachFactory* reachF = (BMPReachFactory* )(it->second);
		return reachF->ReachBMP(reach);
	}
}

BMPReachPointSource* Scenario::getPointSource(int reach)
{
	BMPReach* r= getReachStructure(reach,BMP_TYPE_POINTSOURCE);
	if(r==NULL) return NULL;
	else return (BMPReachPointSource*)r;
}

BMPReachFlowDiversion* Scenario::getFlowDiversion(int reach)
{
	BMPReach* r=  getReachStructure(reach,BMP_TYPE_FLOWDIVERSION_STREAM);
	if(r==NULL) return NULL;
	else return (BMPReachFlowDiversion*)r;
}

BMPReachReservoir* Scenario::getReservoir(int reach)
{
	BMPReach* r=  getReachStructure(reach,BMP_TYPE_RESERVOIR);
	if(r==NULL) return NULL;
	else return (BMPReachReservoir*)r;
}

int Scenario::getMaxReservoirId()
{
	map<int,BMPFactory*>::iterator it;

	it = this->m_bmpFactories.find(BMP_TYPE_RESERVOIR);
	if(it == this->m_bmpFactories.end()) return 0;
	else
	{
		BMPReachFactory* reachF = (BMPReachFactory* )(it->second);
		return reachF->getMaxStructureId();
	}
}

NonStructural::ManagementOperation* Scenario::getOperation(
	int validCellIndex,int startYear,time_t currentTime,int BMPid,bool useSecond)
{
	map<int,BMPFactory*>::iterator it;

	it = this->m_bmpFactories.find(BMPid);
	if(it == this->m_bmpFactories.end()) return NULL;
	else
	{
		BMPArealNonStructural* arealS = (BMPArealNonStructural* )(it->second);
		if(useSecond)	return arealS->getSecondOperation(validCellIndex,startYear,currentTime);
		else			return arealS->getOperation(validCellIndex,startYear,currentTime);
	}
}

}
