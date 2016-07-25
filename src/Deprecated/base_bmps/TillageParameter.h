#pragma once

#include "ArealParameter.h"

namespace MainBMP
{
    namespace BMPParameter
    {
        /*!
         * \class TillageParameter
         * \ingroup BMPParameter
         *
         * \brief Tillage parameter class
         *
         */
        class TillageParameter : public ArealParameter
        {
        public:
            /// Constructor, previous version, with string vector as input
            TillageParameter(vector<string> *oneRow);

            /// Constructor, latest version, with float array as input
            TillageParameter(float *tillageFlds, int count);

            /// Destructor
            ~TillageParameter(void);

            /// Output
            void Dump(ostream *fs);

            /// Mixing efficiency of tillage operation, EFFMIX
            float MixingEfficiency() const { return m_mixingEfficiency; }

            /// Depth of mixing caused by the tillage operation (mm), DEPTIL
            float MixingDepth() const { return m_mixingDepth; }

        private:
            /// Mixing efficiency of tillage operation, EFFMIX
            float m_mixingEfficiency;
            /// Depth of mixing caused by the tillage operation (mm), DEPTIL
            float m_mixingDepth;
        };
    }
}



