/*!
 * \file BMPPlantMgtFactory.h
 * \brief Plant management operations factory
 * \author Liang-Jun Zhu
 * \date June 2016
 */
#pragma once
#include <stdlib.h>
#include "BMPFactory.h"
#include "BMPPlantMgtFactory.h"
#include "PlantManagementOperation.h"
using namespace MainBMP;
using namespace PlantManagement;
namespace MainBMP
{
	/*!
	 * \class BMPPlantMgtFactory
	 * \ingroup MainBMP
	 *
	 * \brief Initiate a plant management BMP
	 * Actually, it contains a series BMPs, such as plant, fertilize, harvest, etc.
	 *
	 */
	class BMPPlantMgtFactory :	public BMPFactory
	{
	public:
		/// Constructor
		BMPPlantMgtFactory(int scenarioId,int bmpId,int subScenario,int bmpType,int bmpPriority,string distribution,string parameter, string location);
		/// Destructor
		~BMPPlantMgtFactory(void);
		/// Load BMP parameters from MongoDB
		void loadBMP(mongoc_client_t* conn, string bmpDBName);
		/// Output
		void Dump(ostream* fs);

	private:
		/// subSecenario name
		string m_name;
		/// parameters
		float* m_parameters;
		/// field index for where to apply the subScenario
		vector<int> m_location;
		/*
		** The first element is the sequence number of plant management operations
		** and the second is the corresponding unique management code
		** m_bmpSequence[0] = 1002 means the first (1001 / 1000 = 1) operation is Irrigation (1002 % 1000 = 2)
		*/
		vector<int> m_bmpSequence;
		/*
		** Key is the unique management code
		** Value is the corresponding PlantMangementOperation instance
		*/
		map<int, PlantManagementOperation*> m_bmpPlantOps;
	};
}
