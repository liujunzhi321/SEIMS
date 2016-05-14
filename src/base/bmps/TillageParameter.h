#pragma once
#include "ArealParameter.h"

namespace MainBMP
{
	namespace BMPParameter
	{
		class TillageParameter :
			public ArealParameter
		{
		public:
			TillageParameter(vector<string> *oneRow);
			~TillageParameter(void);

			void Dump(ostream* fs);

			//Mixing efficiency of tillage operation, EFFMIX
			float MixingEfficiency() const {return m_mixingEfficiency;}
			
			//Depth of mixing caused by the tillage operation (mm), DEPTIL
			float MixingDepth() const {return m_mixingDepth;}
		private:
			//Mixing efficiency of tillage operation, EFFMIX
			float m_mixingEfficiency;
			
			//Depth of mixing caused by the tillage operation (mm), DEPTIL
			float m_mixingDepth;
		};
	}
}



