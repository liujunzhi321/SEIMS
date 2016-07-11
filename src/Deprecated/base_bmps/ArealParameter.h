#pragma once

#include <string>
#include <vector>
#include "ModelException.h"
#include <ostream>

using namespace std;

namespace MainBMP
{
    /*!
     *  \addtogroup BMPParameter
     * \brief BMP related parameters
     */
    namespace BMPParameter
    {
        /*!
         * \class ArealParameter
         * \ingroup BMPParameter
         *
         * \brief Areal BMP parameter class
         *
         */
        class ArealParameter
        {
        public:
            /// Simplified constructor
            ArealParameter(int id);

            /// Constructor
            ArealParameter(int id, string name, string descri);

            /// Constructor
            ArealParameter(vector<string> *oneRow);

            /// Destructor
            ~ArealParameter(void);

            /// Get unique ID of current areal BMP parameter
            int ID() { return m_id; }

            /// Get short name
            string Name() { return m_name; }

            /// Get detailed description
            string Description() { return m_description; }

            // Output
            virtual void Dump(ostream *fs);

        private:
            /// unique ID of current areal BMP parameter
            int m_id;
            /// short name
            string m_name;
            /// detailed description
            string m_description;
        };
    }
}



