#pragma once
#include "ManagementOperation.h"
#include "TillageParameter.h"

namespace MainBMP
{
	namespace NonStructural
	{
		class ManagementOperationTillage :
			public ManagementOperation
		{
		public:
			ManagementOperationTillage(int location,int year, int month, int day,
				BMPParameter::ArealParameter* parameter,
				int code);
			~ManagementOperationTillage(void);

			BMPParameter::TillageParameter* OperationParamter() const {return (BMPParameter::TillageParameter*)(m_parameter);}
		
			void Dump(ostream* fs);
		private:
			int m_tillCode;
		};
	}
}


