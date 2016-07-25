#pragma once

#include "ArealParameter.h"
#include <stdlib.h>

namespace MainBMP
{
    namespace BMPParameter
    {
        /*!
         * \class FertilizerParameter
         * \ingroup BMPParameter
         *
         * \brief Fertilizer parameter class
         *
         */
        class FertilizerParameter :
                public ArealParameter
        {
        public:
            /// Constructor, previous version, with string vector as input
            FertilizerParameter(vector<string> *oneRow);

            /// Constructor, latest version, with float array as input
            FertilizerParameter(float *fertilizerFlds, int count);

            /// Destructor
            ~FertilizerParameter(void);

            /// Output
            void Dump(ostream *fs);

            /// Fraction of mineral N (NO3 and NH4) in fertilizer (kg min-N/kg fertilizer), FMINN
            float MineralNFraction() const { return m_mineralNFraction; }

            /// Fraction of mineral P in fertilizer (kg min-P/kg fertilizer), FMINP
            float MineralPFraction() const { return m_mineralPFraction; }

            /// Fraction of organic N in fertilizer (kg org-N/kg fertilizer), FORGN
            float OrganicNFraction() const { return m_organicNFraction; }

            /// Fraction of organic P in fertilizer (kg org-P/kg fertilizer), FORGP
            float OrganicPFraction() const { return m_organicPFraction; }

            /// Fraction of mineral N in fertilizer applied as ammonia (kg NH3-N/kg min-N), FNH3N
            float NH3Fraction() const { return m_NH3Fraction; }

            /// Concentration of persistent bacteria in manure/fertilizer (# cfu/g manure), BACTPDB
            float PersistentBacteriaFraction() const { return m_persistentBacteriaFraction; }

            /// Concentration of less-persistent bacteria in manure/fertilizer (# cfu/g manure), BACTLPDB
            float NonpersistentBacteriaFraction() const { return m_nonpersistentBacteriaFraction; }

            /// Bacteria partition coefficient, BACTKDDB
            float BacteriaPartitionCoefficient() const { return m_bacteriaPartitionCoefficient; }

        private:
            /// Fraction of mineral N (NO3 and NH4) in fertilizer (kg min-N/kg fertilizer), FMINN
            float m_mineralNFraction;

            /// Fraction of mineral P in fertilizer (kg min-P/kg fertilizer), FMINP
            float m_mineralPFraction;

            /// Fraction of organic N in fertilizer (kg org-N/kg fertilizer), FORGN
            float m_organicNFraction;

            /// Fraction of organic P in fertilizer (kg org-P/kg fertilizer), FORGP
            float m_organicPFraction;

            /// Fraction of mineral N in fertilizer applied as ammonia (kg NH3-N/kg min-N), FNH3N
            float m_NH3Fraction;

            /// Concentration of persistent bacteria in manure/fertilizer (# cfu/g manure), BACTPDB
            float m_persistentBacteriaFraction;

            /// Concentration of less-persistent bacteria in manure/fertilizer (# cfu/g manure), BACTLPDB
            float m_nonpersistentBacteriaFraction;

            /// Bacteria partition coefficient, BACTKDDB
            float m_bacteriaPartitionCoefficient;
        };
    }
}



