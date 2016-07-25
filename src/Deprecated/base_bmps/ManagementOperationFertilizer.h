#pragma once

#include "ManagementOperation.h"
#include "FertilizerParameter.h"

namespace MainBMP
{
    namespace NonStructural
    {
        /*!
         * \class ManagementOperationFertilizer
         * \ingroup NonStructural
         *
         * \brief Fertilize operation class
         *
         */
        class ManagementOperationFertilizer :
                public ManagementOperation
        {
        public:
            /// Constructor, with fertilizer type and operation rate
            ManagementOperationFertilizer(int location, int year, int month, int day,
                                          BMPParameter::ArealParameter *parameter,
                                          int type, int rate);

            /// Destructor
            ~ManagementOperationFertilizer(void);

            /// Get fertilizer parameters
            BMPParameter::FertilizerParameter *OperationParamter() const { return (BMPParameter::FertilizerParameter *) (m_parameter); }

            /// Output
            void Dump(ostream *fs);

        private:
            /// fertilizer type
            int m_ferType;
            /// fertilize rate
            int m_ferRate;
        };
    }
}


