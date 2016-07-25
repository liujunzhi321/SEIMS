#include "BMPReachPointSource.h"

using namespace MainBMP;

BMPReachPointSource::BMPReachPointSource(
        string bmpDatabasePath,
        string parameterTableName,
        int reachId,
        int pointSourceId) : BMPReach(
        bmpDatabasePath,
        parameterTableName,
        BMP_TYPE_POINTSOURCE,
        BMP_NAME_POINTSOURCE,
        reachId,
        pointSourceId)
{

}


BMPReachPointSource::~BMPReachPointSource(void)
{

}

string BMPReachPointSource::ColumnName() { return BMP_NAME_POINTSOURCE; }

bool BMPReachPointSource::HasTimeSeriesData() { return true; }


