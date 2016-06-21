/*!
 * \file BMPFactory.h
 * \brief
 *
 */
#pragma once
#include "BMPReachFlowDiversion.h"
#include "BMPReachPointSource.h"
#include "BMPReachReservoir.h"
#include "MongoUtil.h"
#include "DBManager.h"
#include "BMPText.h"
#include "ModelException.h"
#include <iomanip>

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
		BMPFactory(int scenarioId,int bmpId,int bmpType,string distribution,string parameter);
		/// Destructor
		~BMPFactory(void);
		/// Load BMP parameters from MongoDB
		//virtual void loadBMP(mongoc_client_t* conn) = 0;
		/// Load BMP parameters from SQLite
		virtual void loadBMP(string bmpDatabasePath) = 0;
		/// Get BMP type
		int bmpType(){return m_bmpType;}
		/// Output
		virtual void Dump(ostream* fs) = 0;
	protected:
		/// Scenario ID
		int m_scenarioId;
		/// BMP ID
		int m_bmpId;
		/// BMP Type
		int m_bmpType;
		/// Distribution of BMP
		/// Format is [distribution raster name]/[table name]
		/// in which distribution raster name is a dataset name located in database
		/// and table name is the name of table which defines where, when and how the BMP is applied
		string m_distribution;
		/// The name of the table which defines the parameters, or look up table
		string m_parameter;
	};
}


