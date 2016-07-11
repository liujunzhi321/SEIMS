#pragma once

#include <stdlib.h>
#include "BMPFactory.h"

namespace MainBMP
{
    /*!
     * \class BMPReachFactory
     * \ingroup MainBMP
     *
     * \brief Initiate a Reach BMP
     *
     */
    class BMPReachFactory : public BMPFactory
    {
    public:
        /// Constructor
        BMPReachFactory(int scenarioId, int bmpId, int bmpType, string distribution, string parameter);

        /// Destructor
        ~BMPReachFactory(void);

        /// Load BMP parameters from MongoDB
        //void loadBMP(mongoc_client_t* conn) = 0;
        /// Load BMP parameters from SQLite
        void loadBMP(string bmpDatabasePath);

        /// Create a reach BMP instance
        BMPReach *ReachBMP(int reachId);

        /// Output
        void Dump(ostream *fs);

        /*
        ** Load time series data for some reach structure
        */
        void loadTimeSeriesData(string databasePath, time_t startTime, time_t endTime, int interval);

        float getTimeSeriesData(time_t t, int reach, int dataType);

        int getMaxStructureId();

    private:
        /*
        ** Key is the reach id
        ** Value is the pointer of the BMPReach object
        ** \sa BMPReach
        */
        map<int, BMPReach *> m_bmpReachs;
    };
}



