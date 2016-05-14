#pragma once
#include "ManagementOperation.h"

namespace MainBMP
{
	namespace NonStructural
	{
		class ManagementOperationHarvest :
			public ManagementOperation
		{
		public:
			ManagementOperationHarvest(int location,int year, int month, int day,BMPParameter::ArealParameter* parameter);
			~ManagementOperationHarvest(void);
		};
	}
}



