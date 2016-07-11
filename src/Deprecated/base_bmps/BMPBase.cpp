#include "BMPBase.h"

using namespace MainBMP;

BMPBase::BMPBase(string bmpDatabasePath, int id, string name, int type, string parameter) :
        m_bmpDatabasePath(bmpDatabasePath), m_bmpId(id), m_bmpName(name), m_bmpType(type),
        m_parameterTableName(parameter)
{
}

BMPBase::BMPBase(mongoc_client_t *conn, int id, string name, int type, string parameter) :
        m_conn(conn), m_bmpId(id), m_bmpName(name), m_bmpType(type), m_parameterTableName(parameter)
{
}

BMPBase::~BMPBase(void)
{
}

void BMPBase::Dump(ostream *fs)
{
    if (fs == NULL) return;

    *fs << "---------- BMP:" << m_bmpName
    << ",ID:" << m_bmpId
    << ",Type:" << m_bmpType
    << ",BMPParameter:" << m_parameterTableName
    << "----------" << endl;
}

string BMPBase::ReachBMPColumnNameFromBMPID(int reachbmpId)
{
    if (reachbmpId > BMP_TYPE_RIPARIANBUFFER || reachbmpId < BMP_TYPE_POINTSOURCE) return "";
    switch (reachbmpId)
    {
        case BMP_TYPE_POINTSOURCE:
            return BMP_NAME_POINTSOURCE;
        case BMP_TYPE_FLOWDIVERSION_STREAM:
            return BMP_NAME_FLOWDIVERSION_STREAM;
        case BMP_TYPE_RESERVOIR:
            return BMP_NAME_RESERVOIR;
        case BMP_TYPE_RIPARIANWETLAND:
            return BMP_NAME_RIPARIANWETLAND;
        case BMP_TYPE_RIPARIANBUFFER:
            return BMP_NAME_RIPARIANBUFFER;
        default:
            return "";
    }
}