#pragma once

#include "ManagementOperation.h"

namespace MainBMP
{
    namespace NonStructural
    {
        /*!
         * \class ManagementOperationHarvest
         * \ingroup NonStructural
         *
         * \brief Harvest operation
         *
         */
        class ManagementOperationHarvest :
                public ManagementOperation
        {
        public:
            /// Constructor
            ManagementOperationHarvest(int location, int year, int month, int day,
                                       BMPParameter::ArealParameter *parameter);

            /// Destructor
            ~ManagementOperationHarvest(void);
        };
    }
}



