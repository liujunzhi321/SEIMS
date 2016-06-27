/*!
 * \file BMPFactory.h
 * \brief
 *
 */
#pragma once
#include "MongoUtil.h"
#include "BMPText.h"
#include "ModelException.h"
#include <iomanip>
//#include "DBManager.h"
//#include "BMPReachFlowDiversion.h"
//#include "BMPReachPointSource.h"
//#include "BMPReachReservoir.h"

namespace MainBMP
{
	/*!
	 * \class BMPFactory
	 * \ingroup MainBMP
	 *
	 * \brief Base class to initiate a BMP data
	 *
	 */
	class BMPFactory
	{
	public:
		/// Constructor
		BMPFactory(int scenarioId, int bmpId, int subScenario, int bmpType,int bmpPriority,string distribution,string parameter, string location);
		/// Destructor
		~BMPFactory(void);
		/// Load BMP parameters from MongoDB
		virtual void loadBMP(mongoc_client_t* conn, string bmpDBName) = 0;
		/// Load BMP parameters from SQLite
		///virtual void loadBMP(string bmpDatabasePath) = 0;
		/// Get BMP type
		int bmpType(){return m_bmpType;}
		/// Get subScenario ID
		int GetSubScenarioId(){return m_subScenarioId;}
		/// Output
		virtual void Dump(ostream* fs) = 0;
	protected:
		/// Scenario ID
		int m_scenarioId;
		/// BMP ID
		int m_bmpId;
		/// SubScenario ID within one BMP ID
		int m_subScenarioId;
		/// BMP Type
		int m_bmpType;
		/// BMP Priority
		int m_bmpPriority;
		/// Distribution of BMP
		/// Format is [distribution data type]|[distribution parameter name]
		/// in which distribution data type may be raster or array that stored in database
		string m_distribution;
		/// Collection to 
		string m_bmpCollection;
		/// Define where the BMP will be applied
		string m_location;
	};
}


