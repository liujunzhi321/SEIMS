#pragma once

#include "ManagementOperation.h"
#include "CropParameter.h"

namespace MainBMP
{
    namespace NonStructural
    {
        /*!
         * \class ManagementOperationPlant
         * \ingroup NonStructural
         *
         * \brief Planting operation
         *
         */
        class ManagementOperationPlant :
                public ManagementOperation
        {
        public:
            /// Constructor, with crop code as input
            ManagementOperationPlant(int location, int year, int month, int day,
                                     BMPParameter::ArealParameter *parameter, int crop);

            /// Destructor
            ~ManagementOperationPlant(void);

            /// Get parameters of planting operation
            BMPParameter::CropParameter *OperationParamter() const { return (BMPParameter::CropParameter *) (m_parameter); }

            /// Output to stream
            void Dump(ostream *fs);

        private:
            /// crop to be planted (crop type)
            int m_cropType;
        };
    }
}


