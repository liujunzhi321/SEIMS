#pragma once
#include "ManagementOperation.h"
#include "FertilizerParameter.h"

namespace MainBMP
{
	namespace NonStructural
	{
		class ManagementOperationFertilizer :
			public ManagementOperation
		{
		public:
			ManagementOperationFertilizer(int location,int year, int month, int day,
				BMPParameter::ArealParameter* parameter,
				int type,
				int rate);
			~ManagementOperationFertilizer(void);

			BMPParameter::FertilizerParameter* OperationParamter() const {return (BMPParameter::FertilizerParameter*)(m_parameter);}
		
			void Dump(ostream* fs);
		private:
			int m_ferType;
			int m_ferRate;
		};
	}
}


