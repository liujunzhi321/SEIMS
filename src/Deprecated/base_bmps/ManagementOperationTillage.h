#pragma once

#include "ManagementOperation.h"
#include "TillageParameter.h"

namespace MainBMP
{
    namespace NonStructural
    {
        /*!
         * \class ManagementOperationTillage
         * \ingroup NonStructural
         *
         * \brief Tillage operation class
         *
         */
        class ManagementOperationTillage :
                public ManagementOperation
        {
        public:
            /// Constructor, with tillage code
            ManagementOperationTillage(int location, int year, int month, int day,
                                       BMPParameter::ArealParameter *parameter,
                                       int code);

            /// Destructor
            ~ManagementOperationTillage(void);

            /// Get tillage operation parameters
            BMPParameter::TillageParameter *OperationParamter() const { return (BMPParameter::TillageParameter *) (m_parameter); }

            /// Output
            void Dump(ostream *fs);

        private:
            /// Tillage code
            int m_tillCode;
        };
    }
}


