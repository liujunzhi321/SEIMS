#pragma once
#include <stdlib.h>
#include "BMPFactory.h"

namespace MainBMP
{
	class BMPReachFactory :
		public BMPFactory
	{
	public:
		BMPReachFactory(int scenarioId,int bmpId,int bmpType,string distribution,string parameter);
		~BMPReachFactory(void);

		void loadBMP(string bmpDatabasePath);
		BMPReach* ReachBMP(int reachId);

		void Dump(ostream* fs);

		/*
		** Load time series data for some reach structure
		*/
		void loadTimeSeriesData(string databasePath, time_t startTime, time_t endTime,int interval);
		
		float getTimeSeriesData(time_t t,int reach,int dataType);

		int getMaxStructureId();
	private:
		/*
		** Key is the reach id
		** Value is the pointer of the BMPReach object
		** \sa BMPReach
		*/ 
		map<int,BMPReach*> m_bmpReachs;
	};
}



