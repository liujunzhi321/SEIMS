#pragma once

#include <string>
#include "ArealParameter.h"

using namespace std;

namespace MainBMP
{
    /*!
     *  \addtogroup NonStructural
     * \ingroup MainBMP
     * \brief Area non-structural BMP information
     */
    namespace NonStructural
    {
        /*!
         * \class ManagementOperation
         * \ingroup bmps
         *
         * \brief Base class of management operation of areal non-structural BMP
         *
         */
        class ManagementOperation
        {
        public:
            /*!
             * \brief Constructor
             * \param[in] location Index of field to practice the BMP
             * \param[in] year
             * \param[in] month
             * \param[in] day
             * \param[in] parameter \sa BMPParameter, ArealParameter
             */
            ManagementOperation(int location, int year, int month, int day, BMPParameter::ArealParameter *parameter);

            //! Destructor
            ~ManagementOperation(void);

            /// Get a uniqueID combined by location,year,month and day.
            /// It's used as the key for this operation in class BMPArealNonStructural.
            string UniqueID();

            /// Convert operation information to unique id.
            /// It's used by class ManagementOperation and BMPArealNonStructural when querying.
            static string ManagementOperation2ID(int location, int year, int month, int day);

            /// Output information to stream
            virtual void Dump(ostream *fs);

        protected:
            /// Location index of management spatial unit, e.g., fields
            int m_location;
            /// year
            int m_year;
            /// month
            int m_month;
            /// day
            int m_day;
            /// Areal BMP parameters
            BMPParameter::ArealParameter *m_parameter;
        };
    }
}



