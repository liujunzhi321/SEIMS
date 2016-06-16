#pragma once

#include "BMPReachFlowDiversion.h"
#include "BMPReachPointSource.h"
#include "BMPReachReservoir.h"

#include "DBManager.h"
#include "BMPText.h"
#include "ModelException.h"
#include <iomanip>

namespace MainBMP
{
	class BMPFactory
	{
	public:
		BMPFactory(int scenarioId,int bmpId,int bmpType,string distribution,string parameter);
		~BMPFactory(void);

		virtual void loadBMP(mongoc_client_t* conn) = 0;
		
		int bmpType();

		virtual void Dump(ostream* fs) = 0;
	protected:
		int m_scenarioId;
		int m_bmpId;
		int m_bmpType;
		string m_distribution;
		string m_parameter;
	};
}


