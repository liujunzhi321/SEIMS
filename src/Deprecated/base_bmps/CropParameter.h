#pragma once

#include "ArealParameter.h"

namespace MainBMP
{
    namespace BMPParameter
    {
        /*!
         * \class CropParameter
         * \ingroup BMPParameter
         *
         * \brief Crop parameter class
         *
         */
        class CropParameter :
                public ArealParameter
        {
        public:
            /// Constructor, previous version, with string vector as input
            CropParameter(vector<string> *oneRow);

            /// Constructor, latest version, with float array as input
            CropParameter(float *cropFlds, int count);

            /// Destructor
            ~CropParameter(void);

            /// Output
            void Dump(ostream *fs);

            /// Land cover/plant classification, IDC
            int Claasification() const { return m_classification; }

            /// Radiation-use efficicency or biomass-energy ratio ((kg/ha)/(MJ/m**2)),BIO_E
            float RadiationUseEfficiency() const { return m_radiationUseEfficiency; }

            /// Harvest index for optimal growing conditions, HVSTI
            float HarvestIndex() const { return m_harvestIndex; }

            /// Maximum potential leaf area index, BLAI
            float PotentialLeafAreaIndex() const { return m_potentialLeafAreaIndex; }

            /// Fraction of the plant growing season or fraction of total potential head units corresponding to the 1st point on the optimal leaf area development curve, FRGRW1
            float X1() const { return m_x1; }

            /// Fraction of the maximum leaf area index corresponding to the 1st point on the optimal leaf area development curve, LAIMX1
            float Y1() const { return m_y1; }

            /// Fraction of the plant growing season or fraction of total potential head units corresponding to the 2nd point on the optimal leaf area development curve, FRGRW2
            float X2() const { return m_x2; }

            /// Fraction of the maximum leaf area index corresponding to the 2nd point on the optimal leaf area development curve, LAIMX2
            float Y2() const { return m_y2; }

            /// Fraction of growing season leaf area index begins to decline, DLAI
            float DeclineLAIFraction() const { return m_declineLAIFraction; }

            /// Maximum canopy height (m), CHTMX
            float MaxHeight() const { return m_maxHeight; }

            /// Maximum root depth (m), RDMX
            float MaxRoot() const { return m_maxRoot; }

            /// Optimal temperature for plant growth (0C), T_OPT
            float OptTemperature() const { return m_optTemperature; }

            /// Minimum (base) temperature for plant growth (0C), T_BASE
            float MinTemperature() const { return m_minTemperature; }

            /// Normal fraction of nitrogen in yield (kg N/kg yield), CNYLD
            float YieldNFraction() const { return m_yieldNFraction; }

            /// Normal fraction of phosphorus in yield (kg P/kg yield), CPYLD
            float YieldPFraction() const { return m_yieldPFraction; }

            /// Nitrogen uptake parameter #1: normal fraction of nitrogen in plant biomass at emergence (kg N/kg biomass), BN1
            float BiomassNFraction1() const { return m_biomassNFraction1; }

            /// Nitrogen uptake parameter #2: normal fraction of nitrogen in plant biomass at 50% maturity (kg N/kg biomass), BN2
            float iomassNFraction2() const { return m_biomassNFraction2; }

            /// Nitrogen uptake parameter #3: normal fraction of nitrogen in plant biomass at maturity (kg N/kg biomass), BN3
            float BiomassNFraction3() const { return m_biomassNFraction3; }

            /// Nitrogen uptake parameter #1: normal fraction of phosphorus in plant biomass at emergence (kg P/kg biomass), BP1
            float BiomassPFraction1() const { return m_biomassPFraction1; }

            /// Nitrogen uptake parameter #2: normal fraction of phosphorus in plant biomass at 50% maturity (kg P/kg biomass), BP2
            float BiomassPFraction2() const { return m_biomassPFraction2; }

            /// Nitrogen uptake parameter #3: normal fraction of phosphorus in plant biomass at maturity (kg P/kg biomass), BP3
            float BiomassPFraction3() const { return m_biomassPFraction3; }

            /// Lower limit of harvest index ((kg/ha)/(kg/ha)), WSYF
            float HarvestIndexLowerLimit() const { return m_harvestIndexLowerLimit; }

            /// Minimum value of USLE_C factor for water erosion applicable to the land cover/plant
            float MinUSLEC() const { return m_minUSLEC; }

            /// Maximum stomatal conductance at high solar radiation and low vapor pressure deficit (m.s-1), GSI
            float MaxStomatalConductance() { return m_maxStomatalConductance; }

            /// Vaper pressure dificit (kPa) corresponding to the second point on the stomatal conductance curve, VPDFR
            float SecondVPD() const { return m_secondVPD; }

            /// Fraction of maximum stomatal conductance corresponding to the second point on the stomatal conductance curve, FRGMAX
            float SecondStomatalConductance() const { return m_secondStomatalConductance; }

            /// Rate of decline in radiation use efficiency per unit increace in vapor pressure dificit, WAVP
            float RadiationUseEfficiencyDeclineRateWithVPD() const { return m_radiationUseEfficiencyDeclineRateWithVPD; }

            /// Elevated CO2 atmospheric concentration (uL CO2/L air) corresponding the 2nd on the radition use efficiency curve, CO2HI
            float SecondCO2Concentration() const { return m_secondCO2Concentration; }

            /// Biomass-energy ratio corresponding to the 2nd point on the radiation use efficiency curve, BIOEHI
            float SecondBiomassEnergyRatio() const { return m_secondBiomassEnergyRatio; }

            /// Plant residue decomposition coefficient, RSDCO_PL
            float PlantResidueDecompositionCoefficient() const { return m_plantResidueDecompositionCoefficient; }

            /// Default Manning's 'n' value for overland flow for land cover/plant, OV_N
            float DefaultManningN() const { return m_defaultManningN; }

            /// Default SCS CN value for moisure condition II used for the land cover/plant where the plant is growing on a soil with a hydrologic group classification of A
            float CN2A() const { return m_CN2A; }

            /// Default SCS CN value for moisure condition II used for the land cover/plant where the plant is growing on a soil with a hydrologic group classification of B
            float CN2B() const { return m_CN2B; }

            /// Default SCS CN value for moisure condition II used for the land cover/plant where the plant is growing on a soil with a hydrologic group classification of C
            float CN2C() const { return m_CN2C; }

            /// Default SCS CN value for moisure condition II used for the land cover/plant where the plant is growing on a soil with a hydrologic group classification of D
            float CN2D() const { return m_CN2D; }

            /// Fertilizer flag, FERTFIELD
            /// 1. include auto-fertilizer operation in default mgt file
            /// 2. do not include auto-fertilizer operation in default mgt file
            bool IsAutoFertilizer() const { return m_isAutoFertilizer; }

            /// Minimum leaf area index for plant during dormant period (m**2/m**2), ALAI_MIN
            float MinDormantLAI() const { return m_minDormantLAI; }

            /// Fraction of tree biomass accumulated each year that is converted to residue during dormancy, BIO_LEAF
            float TreeDormantResidueBiomassFraction() const { return m_treeDormantResidueBiomassFraction; }

            /// Number of years required for tree species to reach full development (years), MAT_YRS
            int TreeFullDevelopmentYear() const { return m_treeFullDevelopmentYear; }

            /// Maximum biomass for a forest (metric tons/ha), BMX_TREES
            float MaxBiomass() const { return m_maxBiomass; }

            /// Light extinction coefficient, EXT_COEF
            float LightExtinctionCoefficient() const { return m_lightExtinctionCoefficient; }

        private:
            /// Land cover/plant classification, IDC
            int m_classification;

            /// Radiation-use efficicency or biomass-energy ratio ((kg/ha)/(MJ/m**2)),BIO_E
            float m_radiationUseEfficiency;

            /// Harvest index for optimal growing conditions, HVSTI
            float m_harvestIndex;

            /// Maximum potential leaf area index, BLAI
            float m_potentialLeafAreaIndex;

            /// Fraction of the plant growing season or fraction of total potential head units corresponding to the 1st point on the optimal leaf area development curve, FRGRW1
            float m_x1;

            /// Fraction of the maximum leaf area index corresponding to the 1st point on the optimal leaf area development curve, LAIMX1
            float m_y1;

            /// Fraction of the plant growing season or fraction of total potential head units corresponding to the 2nd point on the optimal leaf area development curve, FRGRW2
            float m_x2;

            /// Fraction of the maximum leaf area index corresponding to the 2nd point on the optimal leaf area development curve, LAIMX2
            float m_y2;

            /// Fraction of growing season leaf area index begins to decline, DLAI
            float m_declineLAIFraction;

            /// Maximum canopy height (m), CHTMX
            float m_maxHeight;

            /// Maximum root depth (m), RDMX
            float m_maxRoot;

            /// Optimal temperature for plant growth (0C), T_OPT
            float m_optTemperature;

            /// Minimum (base) temperature for plant growth (0C), T_BASE
            float m_minTemperature;

            /// Normal fraction of nitrogen in yield (kg N/kg yield), CNYLD
            float m_yieldNFraction;

            /// Normal fraction of phosphorus in yield (kg P/kg yield), CPYLD
            float m_yieldPFraction;

            /// Nitrogen uptake parameter #1: normal fraction of nitrogen in plant biomass at emergence (kg N/kg biomass), BN1
            float m_biomassNFraction1;

            /// Nitrogen uptake parameter #2: normal fraction of nitrogen in plant biomass at 50% maturity (kg N/kg biomass), BN2
            float m_biomassNFraction2;

            /// Nitrogen uptake parameter #3: normal fraction of nitrogen in plant biomass at maturity (kg N/kg biomass), BN3
            float m_biomassNFraction3;

            /// Nitrogen uptake parameter #1: normal fraction of phosphorus in plant biomass at emergence (kg P/kg biomass), BP1
            float m_biomassPFraction1;

            /// Nitrogen uptake parameter #2: normal fraction of phosphorus in plant biomass at 50% maturity (kg P/kg biomass), BP2
            float m_biomassPFraction2;

            /// Nitrogen uptake parameter #3: normal fraction of phosphorus in plant biomass at maturity (kg P/kg biomass), BP3
            float m_biomassPFraction3;

            /// Lower limit of harvest index ((kg/ha)/(kg/ha)), WSYF
            float m_harvestIndexLowerLimit;

            /// Minimum value of USLE_C factor for water erosion applicable to the land cover/plant
            float m_minUSLEC;

            /// Maximum stomatal conductance at high solar radiation and low vapor pressure deficit (m.s-1), GSI
            float m_maxStomatalConductance;

            /// Vaper pressure dificit (kPa) corresponding to the second point on the stomatal conductance curve, VPDFR
            float m_secondVPD;

            /// Fraction of maximum stomatal conductance corresponding to the second point on the stomatal conductance curve, FRGMAX
            float m_secondStomatalConductance;

            /// Rate of decline in radiation use efficiency per unit increace in vapor pressure dificit, WAVP
            float m_radiationUseEfficiencyDeclineRateWithVPD;

            /// Elevated CO2 atmospheric concentration (uL CO2/L air) corresponding the 2nd on the radition use efficiency curve, CO2HI
            float m_secondCO2Concentration;

            /// Biomass-energy ratio corresponding to the 2nd point on the radiation use efficiency curve, BIOEHI
            float m_secondBiomassEnergyRatio;

            /// Plant residue decomposition coefficient, RSDCO_PL
            float m_plantResidueDecompositionCoefficient;

            //-------- OV_N,CN2A,CN2B,CN2C,CN2D,FERTFIELD just existing in ArcSWAT ----------

            /// Default Manning's 'n' value for overland flow for land cover/plant, OV_N
            float m_defaultManningN;

            /// Default SCS CN value for moisure condition II used for the land cover/plant where the plant is growing on a soil with a hydrologic group classification of A
            float m_CN2A;

            /// Default SCS CN value for moisure condition II used for the land cover/plant where the plant is growing on a soil with a hydrologic group classification of B
            float m_CN2B;

            /// Default SCS CN value for moisure condition II used for the land cover/plant where the plant is growing on a soil with a hydrologic group classification of C
            float m_CN2C;

            /// Default SCS CN value for moisure condition II used for the land cover/plant where the plant is growing on a soil with a hydrologic group classification of D
            float m_CN2D;

            /// Fertilizer flag, FERTFIELD
            /// 1. include auto-fertilizer operation in default mgt file
            /// 2. do not include auto-fertilizer operation in default mgt file
            bool m_isAutoFertilizer;

            //-------------------------------------------------------------------------------

            /// Minimum leaf area index for plant during dormant period (m**2/m**2), ALAI_MIN
            float m_minDormantLAI;

            /// Fraction of tree biomass accumulated each year that is converted to residue during dormancy, BIO_LEAF
            float m_treeDormantResidueBiomassFraction;

            /// Number of years required for tree species to reach full development (years), MAT_YRS
            int m_treeFullDevelopmentYear;

            /// Maximum biomass for a forest (metric tons/ha), BMX_TREES
            float m_maxBiomass;

            /// Light extinction coefficient, EXT_COEF
            float m_lightExtinctionCoefficient;
        };
    }
}


