#include "BMPFactory.h"
using namespace MainBMP;

BMPFactory::BMPFactory(int scenarioId,int bmpId,int subScenario,int bmpType, int bmpPriority,string distribution,string parameter,string location):
m_scenarioId(scenarioId),m_bmpId(bmpId),m_subScenarioId(subScenario),m_bmpType(bmpType),m_bmpPriority(bmpPriority),
	m_distribution(distribution),m_bmpCollection(parameter),m_location(location)
{
}

BMPFactory::~BMPFactory(void)
{
}