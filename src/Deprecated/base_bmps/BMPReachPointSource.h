#pragma once

#include "BMPReach.h"

namespace MainBMP
{
    class BMPReachPointSource :
            public BMPReach
    {
    public:
        BMPReachPointSource(
                string bmpDatabasePath,
                string parameterTableName,
                int reachId,
                int pointSourceId);

        ~BMPReachPointSource(void);

        string ColumnName();

        bool HasTimeSeriesData();
    };
}



