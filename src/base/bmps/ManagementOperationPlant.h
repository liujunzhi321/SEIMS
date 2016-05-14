#pragma once
#include "ManagementOperation.h"
#include "CropParameter.h"

namespace MainBMP
{
	namespace NonStructural
	{
		class ManagementOperationPlant :
			public ManagementOperation
		{
		public:
			ManagementOperationPlant(int location,int year, int month, int day,
				BMPParameter::ArealParameter* parameter,
				int crop);
			~ManagementOperationPlant(void);

			BMPParameter::CropParameter* OperationParamter() const {return (BMPParameter::CropParameter*)(m_parameter);}
		
			void Dump(ostream* fs);
		private:
			int m_cropType;
		};
	}
}


