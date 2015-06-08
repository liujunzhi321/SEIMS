#pragma once

#include <string>
#include <vector>
#include "ModelException.h"
#include <ostream>

using namespace std;

namespace MainBMP
{
	namespace BMPParameter
	{
		class ArealParameter
		{
		public:
			ArealParameter(int id,string name,string descri);
			ArealParameter(vector<string> *oneRow);
			~ArealParameter(void);

			int ID()				{return m_id;}
			string Name()			{return m_name;}
			string Description()	{return m_description;}

			virtual void Dump(ostream* fs);
		private:
			//unique ID
			int m_id;

			//short name
			string m_name;

			//detailed description
			string m_description;
		};
	}
}



