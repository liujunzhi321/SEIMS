#include "BMPFactory.h"
using namespace MainBMP;

BMPFactory::BMPFactory(int scenarioId,int bmpId,int bmpType,string distribution,string parameter):
m_scenarioId(scenarioId),m_bmpId(bmpId),m_bmpType(bmpType),m_distribution(distribution),m_parameter(parameter)
{
}

BMPFactory::~BMPFactory(void)
{
}