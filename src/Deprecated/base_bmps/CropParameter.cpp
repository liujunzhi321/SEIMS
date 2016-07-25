#include "CropParameter.h"
#include <stdlib.h>

using namespace MainBMP;
using namespace BMPParameter;

#define CROP_PARAM_COUNT                48
/// Index started with 0
/// ICNUM, IDC,BIO_E, HVSTI, BLAI, FRGRW1, LAIMX1, FRGRW2, LAIMX2, DLAI, CHTMX,     0-10
/// RDMX, T_OPT, T_BASE, CNYLD, CPYLD, BN1, BN2, BN3, BP1, BP2,    11-20
/// BP3, WSYF, USLE_C, GSI, VPDFR, FRGMAX, WAVP, CO2HI, BIOEHI, RSDCO_PL,  21-30
/// OV_N, CN2A, CN2B, CN2C, CN2D, FERTFIELD, ALAI_MIN, BIO_LEAF, MAT_YRS, BMX_TREES, 31-40
/// EXT_COEF, BM_DIEOFF, PHU, CNOP, LAI_INIT, BIO_INIT, CURYR_INIT 41-47
#define    CROP_PARAM_IDX_IDC            1
#define    CROP_PARAM_IDX_BIO_E        2
#define    CROP_PARAM_IDX_HVSTI        3
#define    CROP_PARAM_IDX_BLAI            4
#define    CROP_PARAM_IDX_FRGRW1    5
#define    CROP_PARAM_IDX_LAIMX1        6
#define    CROP_PARAM_IDX_FRGRW2    7
#define    CROP_PARAM_IDX_LAIMX2        8
#define    CROP_PARAM_IDX_DLAI            9
#define    CROP_PARAM_IDX_CHTMX        10
#define    CROP_PARAM_IDX_RDMX        11
#define    CROP_PARAM_IDX_T_OPT        12
#define    CROP_PARAM_IDX_T_BASE    13
#define    CROP_PARAM_IDX_CNYLD        14
#define    CROP_PARAM_IDX_CPYLD        15
#define    CROP_PARAM_IDX_BN1            16
#define    CROP_PARAM_IDX_BN2    17
#define    CROP_PARAM_IDX_BN3    18
#define    CROP_PARAM_IDX_BP1    19
#define    CROP_PARAM_IDX_BP2    20
#define    CROP_PARAM_IDX_BP3    21
#define    CROP_PARAM_IDX_WSYF    22
#define    CROP_PARAM_IDX_USLE_C    23
#define    CROP_PARAM_IDX_GSI    24
#define    CROP_PARAM_IDX_VPDFR    25
#define    CROP_PARAM_IDX_FRGMAX    26
#define    CROP_PARAM_IDX_WAVP    27
#define    CROP_PARAM_IDX_CO2HI    28
#define    CROP_PARAM_IDX_BIOEHI    29
#define    CROP_PARAM_IDX_RSDCO_PL    30
#define    CROP_PARAM_IDX_OV_N    31
#define    CROP_PARAM_IDX_CN2A    32
#define    CROP_PARAM_IDX_CN2B    33
#define    CROP_PARAM_IDX_CN2C    34
#define    CROP_PARAM_IDX_CN2D    35
#define    CROP_PARAM_IDX_FERTFIELD    36
#define    CROP_PARAM_IDX_ALAI_MIN    37
#define    CROP_PARAM_IDX_BIO_LEAF    38

CropParameter::CropParameter(vector<string> *oneRow) : ArealParameter(oneRow)
{
    if (int(oneRow->size()) < CROP_PARAM_COUNT)
        throw ModelException("CropParameter", "CropParameter", "oneRow should have 41 parameters at least!");

    m_classification = atoi(oneRow->at(CROP_PARAM_IDX_IDC).c_str());
    m_radiationUseEfficiency = (float) atof(oneRow->at(CROP_PARAM_IDX_BIO_E).c_str());
    m_harvestIndex = (float) atof(oneRow->at(CROP_PARAM_IDX_HVSTI).c_str());
    m_potentialLeafAreaIndex = (float) atof(oneRow->at(CROP_PARAM_IDX_BLAI).c_str());
    m_x1 = (float) atof(oneRow->at(CROP_PARAM_IDX_FRGRW1).c_str());
    m_y1 = (float) atof(oneRow->at(CROP_PARAM_IDX_LAIMX1).c_str());
    m_x2 = (float) atof(oneRow->at(CROP_PARAM_IDX_FRGRW2).c_str());
    m_y2 = (float) atof(oneRow->at(CROP_PARAM_IDX_LAIMX2).c_str());
    m_declineLAIFraction = (float) atof(oneRow->at(CROP_PARAM_IDX_DLAI).c_str());
    m_maxHeight = (float) atof(oneRow->at(CROP_PARAM_IDX_CHTMX).c_str());
    m_maxRoot = (float) atof(oneRow->at(CROP_PARAM_IDX_RDMX).c_str());
    m_optTemperature = (float) atof(oneRow->at(CROP_PARAM_IDX_T_OPT).c_str());
    m_minTemperature = (float) atof(oneRow->at(CROP_PARAM_IDX_T_BASE).c_str());
    m_yieldNFraction = (float) atof(oneRow->at(CROP_PARAM_IDX_CNYLD).c_str());
    m_yieldPFraction = (float) atof(oneRow->at(CROP_PARAM_IDX_CPYLD).c_str());
    m_biomassNFraction1 = (float) atof(oneRow->at(CROP_PARAM_IDX_BN1).c_str());
    m_biomassNFraction2 = (float) atof(oneRow->at(CROP_PARAM_IDX_BN2).c_str());
    m_biomassNFraction3 = (float) atof(oneRow->at(CROP_PARAM_IDX_BN3).c_str());
    m_biomassPFraction1 = (float) atof(oneRow->at(CROP_PARAM_IDX_BP1).c_str());
    m_biomassPFraction2 = (float) atof(oneRow->at(CROP_PARAM_IDX_BP2).c_str());
    m_biomassPFraction3 = (float) atof(oneRow->at(CROP_PARAM_IDX_BP3).c_str());
    m_harvestIndexLowerLimit = (float) atof(oneRow->at(CROP_PARAM_IDX_WSYF).c_str());
    m_minUSLEC = (float) atof(oneRow->at(CROP_PARAM_IDX_USLE_C).c_str());
    m_maxStomatalConductance = (float) atof(oneRow->at(CROP_PARAM_IDX_GSI).c_str());
    m_secondVPD = (float) atof(oneRow->at(CROP_PARAM_IDX_VPDFR).c_str());
    m_secondStomatalConductance = (float) atof(oneRow->at(CROP_PARAM_IDX_FRGMAX).c_str());
    m_radiationUseEfficiencyDeclineRateWithVPD = (float) atof(oneRow->at(CROP_PARAM_IDX_WAVP).c_str());
    m_secondCO2Concentration = (float) atof(oneRow->at(CROP_PARAM_IDX_CO2HI).c_str());
    m_secondBiomassEnergyRatio = (float) atof(oneRow->at(CROP_PARAM_IDX_BIOEHI).c_str());
    m_plantResidueDecompositionCoefficient = (float) atof(oneRow->at(CROP_PARAM_IDX_RSDCO_PL).c_str());
    m_defaultManningN = (float) atof(oneRow->at(CROP_PARAM_IDX_OV_N).c_str());
    m_CN2A = (float) atof(oneRow->at(CROP_PARAM_IDX_CN2A).c_str());
    m_CN2B = (float) atof(oneRow->at(CROP_PARAM_IDX_CN2B).c_str());
    m_CN2C = (float) atof(oneRow->at(CROP_PARAM_IDX_CN2C).c_str());
    m_CN2D = (float) atof(oneRow->at(CROP_PARAM_IDX_CN2D).c_str());
    m_isAutoFertilizer = (atoi(oneRow->at(CROP_PARAM_IDX_FERTFIELD).c_str()) != 0 ? true : false);
    m_minDormantLAI = (float) atof(oneRow->at(CROP_PARAM_IDX_ALAI_MIN).c_str());
    m_treeDormantResidueBiomassFraction = (float) atof(oneRow->at(CROP_PARAM_IDX_BIO_LEAF).c_str());
}

CropParameter::CropParameter(float *cropFlds, int count) : ArealParameter((int) cropFlds[0])
{
    if (count != CROP_PARAM_COUNT)
        throw ModelException("CropParameter", "CropParameter", "Crop parameters should have 48 parameters!");

    m_classification = (int) cropFlds[CROP_PARAM_IDX_IDC];
    m_radiationUseEfficiency = cropFlds[CROP_PARAM_IDX_BIO_E];
    m_harvestIndex = cropFlds[CROP_PARAM_IDX_HVSTI];
    m_potentialLeafAreaIndex = cropFlds[CROP_PARAM_IDX_BLAI];
    m_x1 = cropFlds[CROP_PARAM_IDX_FRGRW1];
    m_y1 = cropFlds[CROP_PARAM_IDX_LAIMX1];
    m_x2 = cropFlds[CROP_PARAM_IDX_FRGRW2];
    m_y2 = cropFlds[CROP_PARAM_IDX_LAIMX2];
    m_declineLAIFraction = cropFlds[CROP_PARAM_IDX_DLAI];
    m_maxHeight = cropFlds[CROP_PARAM_IDX_CHTMX];
    m_maxRoot = cropFlds[CROP_PARAM_IDX_RDMX];
    m_optTemperature = cropFlds[CROP_PARAM_IDX_T_OPT];
    m_minTemperature = cropFlds[CROP_PARAM_IDX_T_BASE];
    m_yieldNFraction = cropFlds[CROP_PARAM_IDX_CNYLD];
    m_yieldPFraction = cropFlds[CROP_PARAM_IDX_CPYLD];
    m_biomassNFraction1 = cropFlds[CROP_PARAM_IDX_BN1];
    m_biomassNFraction2 = cropFlds[CROP_PARAM_IDX_BN2];
    m_biomassNFraction3 = cropFlds[CROP_PARAM_IDX_BN3];
    m_biomassPFraction1 = cropFlds[CROP_PARAM_IDX_BP1];
    m_biomassPFraction2 = cropFlds[CROP_PARAM_IDX_BP2];
    m_biomassPFraction3 = cropFlds[CROP_PARAM_IDX_BP3];
    m_harvestIndexLowerLimit = cropFlds[CROP_PARAM_IDX_WSYF];
    m_minUSLEC = cropFlds[CROP_PARAM_IDX_USLE_C];
    m_maxStomatalConductance = cropFlds[CROP_PARAM_IDX_GSI];
    m_secondVPD = cropFlds[CROP_PARAM_IDX_VPDFR];
    m_secondStomatalConductance = cropFlds[CROP_PARAM_IDX_FRGMAX];
    m_radiationUseEfficiencyDeclineRateWithVPD = cropFlds[CROP_PARAM_IDX_WAVP];
    m_secondCO2Concentration = cropFlds[CROP_PARAM_IDX_CO2HI];
    m_secondBiomassEnergyRatio = cropFlds[CROP_PARAM_IDX_BIOEHI];
    m_plantResidueDecompositionCoefficient = cropFlds[CROP_PARAM_IDX_RSDCO_PL];
    m_defaultManningN = cropFlds[CROP_PARAM_IDX_OV_N];
    m_CN2A = cropFlds[CROP_PARAM_IDX_CN2A];
    m_CN2B = cropFlds[CROP_PARAM_IDX_CN2B];
    m_CN2C = cropFlds[CROP_PARAM_IDX_CN2C];
    m_CN2D = cropFlds[CROP_PARAM_IDX_CN2D];
    m_isAutoFertilizer = (cropFlds[CROP_PARAM_IDX_FERTFIELD] != 0 ? true : false);
    m_minDormantLAI = cropFlds[CROP_PARAM_IDX_ALAI_MIN];
    m_treeDormantResidueBiomassFraction = cropFlds[CROP_PARAM_IDX_BIO_LEAF];
}

CropParameter::~CropParameter(void)
{
}

void CropParameter::Dump(ostream *fs)
{
    if (fs == NULL) return;

    ArealParameter::Dump(fs);

    *fs << "IDC:" << m_classification << endl;
    *fs << "BIO_E:" << m_radiationUseEfficiency << endl;
    *fs << "HVSTI:" << m_harvestIndex << endl;
    *fs << "BALI:" << m_potentialLeafAreaIndex << endl;
    *fs << "FRGRW1:" << m_x1 << endl;
    *fs << "LAIMX1:" << m_y1 << endl;
    *fs << "FRGRW2:" << m_x2 << endl;
    *fs << "LAIMX2:" << m_y2 << endl;
    *fs << "DLAI:" << m_declineLAIFraction << endl;
    *fs << "CHTMX:" << m_maxHeight << endl;
    *fs << "RDMX:" << m_maxRoot << endl;
    *fs << "T_OPT:" << m_optTemperature << endl;
    *fs << "T_BASE:" << m_minTemperature << endl;
    *fs << "CNYLD:" << m_yieldNFraction << endl;
    *fs << "CPYLD:" << m_yieldPFraction << endl;
    *fs << "BN1:" << m_biomassNFraction1 << endl;
    *fs << "BN2:" << m_biomassNFraction2 << endl;
    *fs << "BN3:" << m_biomassNFraction3 << endl;
    *fs << "BP1:" << m_biomassPFraction1 << endl;
    *fs << "BP2:" << m_biomassPFraction2 << endl;
    *fs << "BP3:" << m_biomassPFraction3 << endl;
    *fs << "WSYF:" << m_harvestIndexLowerLimit << endl;
    *fs << "USLE_C:" << m_minUSLEC << endl;
    *fs << "GSI:" << m_maxStomatalConductance << endl;
    *fs << "VPDFR:" << m_secondVPD << endl;
    *fs << "FRGMAX:" << m_secondStomatalConductance << endl;
    *fs << "WAVP:" << m_radiationUseEfficiencyDeclineRateWithVPD << endl;
    *fs << "CO2HI:" << m_secondCO2Concentration << endl;
    *fs << "BIOEHI:" << m_secondBiomassEnergyRatio << endl;
    *fs << "RSDCO_PL:" << m_plantResidueDecompositionCoefficient << endl;
    *fs << "OV_N:" << m_defaultManningN << endl;
    *fs << "CN2A:" << m_CN2A << endl;
    *fs << "CN2B:" << m_CN2B << endl;
    *fs << "CN2C:" << m_CN2C << endl;
    *fs << "CN2D:" << m_CN2D << endl;
    *fs << "FERTFEILD:" << m_isAutoFertilizer << endl;
    *fs << "ALAI_MIN:" << m_minDormantLAI << endl;
    *fs << "BIO_LEAF:" << m_treeDormantResidueBiomassFraction << endl;
}
