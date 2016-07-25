#pragma once

#include "BMPReach.h"

namespace MainBMP
{
    class BMPReachFlowDiversion :
            public BMPReach
    {
    public:
        BMPReachFlowDiversion(
                string bmpDatabasePath,
                string parameterTableName,
                int reachId,
                int flowDiversionId);

        ~BMPReachFlowDiversion(void);

        string ColumnName();

        bool HasTimeSeriesData();
    };
}


