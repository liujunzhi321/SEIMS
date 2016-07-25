#include "BMPReachFlowDiversion.h"

using namespace MainBMP;

BMPReachFlowDiversion::BMPReachFlowDiversion(
        string bmpDatabasePath,
        string parameterTableName,
        int reachId,
        int flowDiversionId) : BMPReach(
        bmpDatabasePath,
        parameterTableName,
        BMP_TYPE_FLOWDIVERSION_STREAM,
        BMP_NAME_FLOWDIVERSION_STREAM,
        reachId,
        flowDiversionId)
{
}


BMPReachFlowDiversion::~BMPReachFlowDiversion(void)
{
}

string BMPReachFlowDiversion::ColumnName() { return BMP_NAME_FLOWDIVERSION_STREAM; }

bool BMPReachFlowDiversion::HasTimeSeriesData() { return true; }
