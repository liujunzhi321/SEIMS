#pragma once
#include <string>
#include "ArealParameter.h"

using namespace std;

namespace MainBMP
{
	namespace NonStructural
	{
		/*
		** Base class of management operation of areal non-structural BMP
		*/
		class ManagementOperation
		{
		public:
			ManagementOperation(int location,int year, int month, int day,BMPParameter::ArealParameter* parameter);
			~ManagementOperation(void);

			//This uniqueID is combined by location,year,month and day.
			//It's used as the key for this operation in clas BMPArealNonStructural.
			string UniqueID();

			//Convert operation informatio to unique id.
			//It's used by class ManagementOperation and BMPArealNonStructural when querying.
			static string ManagementOperation2ID(int location,int year, int month, int day);

			virtual void Dump(ostream* fs);
		protected:
			int m_location;
			int m_year;
			int m_month;
			int m_day;
			
			BMPParameter::ArealParameter* m_parameter;
		};
	}
}



