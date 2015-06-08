#include "CropParameter.h"
#include <stdlib.h>

using namespace MainBMP;
using namespace BMPParameter;

#define CROP_PARAMETER_COUNT 41 //?

#define 	CROP_PARAMETER_INDEX_IDC	3
#define 	CROP_PARAMETER_INDEX_BIO_E	4
#define 	CROP_PARAMETER_INDEX_HVSTI	5
#define 	CROP_PARAMETER_INDEX_BLAI	6
#define 	CROP_PARAMETER_INDEX_FRGRW1	7
#define 	CROP_PARAMETER_INDEX_LAIMX1	8
#define 	CROP_PARAMETER_INDEX_FRGRW2	9
#define 	CROP_PARAMETER_INDEX_LAIMX2	10
#define 	CROP_PARAMETER_INDEX_DLAI	11
#define 	CROP_PARAMETER_INDEX_CHTMX	12
#define 	CROP_PARAMETER_INDEX_RDMX	13
#define 	CROP_PARAMETER_INDEX_T_OPT	14
#define 	CROP_PARAMETER_INDEX_T_BASE	15
#define 	CROP_PARAMETER_INDEX_CNYLD	16
#define 	CROP_PARAMETER_INDEX_CPYLD	17
#define 	CROP_PARAMETER_INDEX_BN1	18
#define 	CROP_PARAMETER_INDEX_BN2	19
#define 	CROP_PARAMETER_INDEX_BN3	20
#define 	CROP_PARAMETER_INDEX_BP1	21
#define 	CROP_PARAMETER_INDEX_BP2	22
#define 	CROP_PARAMETER_INDEX_BP3	23
#define 	CROP_PARAMETER_INDEX_WSYF	24
#define 	CROP_PARAMETER_INDEX_USLE_C	25
#define 	CROP_PARAMETER_INDEX_GSI	26
#define 	CROP_PARAMETER_INDEX_VPDFR	27
#define 	CROP_PARAMETER_INDEX_FRGMAX	28
#define 	CROP_PARAMETER_INDEX_WAVP	29
#define 	CROP_PARAMETER_INDEX_CO2HI	30
#define 	CROP_PARAMETER_INDEX_BIOEHI	31
#define 	CROP_PARAMETER_INDEX_RSDCO_PL	32
#define 	CROP_PARAMETER_INDEX_OV_N	33
#define 	CROP_PARAMETER_INDEX_CN2A	34
#define 	CROP_PARAMETER_INDEX_CN2B	35
#define 	CROP_PARAMETER_INDEX_CN2C	36
#define 	CROP_PARAMETER_INDEX_CN2D	37
#define 	CROP_PARAMETER_INDEX_FERTFIELD	38
#define 	CROP_PARAMETER_INDEX_ALAI_MIN	39
#define 	CROP_PARAMETER_INDEX_BIO_LEAF	40

CropParameter::CropParameter(vector<string> *oneRow):ArealParameter(oneRow)
{
	if(int(oneRow->size()) < CROP_PARAMETER_COUNT)	throw ModelException("CropParameter","CropParameter","oneRow should have 41 parameters at least!");
	
	m_classification =			atoi(oneRow->at(CROP_PARAMETER_INDEX_IDC).c_str());
	m_radiationUseEfficiency =	(float)atof(oneRow->at(CROP_PARAMETER_INDEX_BIO_E).c_str());
	m_harvestIndex =				(float)atof(oneRow->at(CROP_PARAMETER_INDEX_HVSTI).c_str());
	m_potentialLeafAreaIndex =	(float)atof(oneRow->at(CROP_PARAMETER_INDEX_BLAI).c_str());
	m_x1 =						(float)atof(oneRow->at(CROP_PARAMETER_INDEX_FRGRW1).c_str());
	m_y1 =						(float)atof(oneRow->at(CROP_PARAMETER_INDEX_LAIMX1).c_str());
	m_x2 =						(float)atof(oneRow->at(CROP_PARAMETER_INDEX_FRGRW2).c_str());
	m_y2 =						(float)atof(oneRow->at(CROP_PARAMETER_INDEX_LAIMX2).c_str());
	m_declineLAIFraction =		(float)atof(oneRow->at(CROP_PARAMETER_INDEX_DLAI).c_str());
	m_maxHeight =					(float)atof(oneRow->at(CROP_PARAMETER_INDEX_CHTMX).c_str());
	m_maxRoot =					(float)atof(oneRow->at(CROP_PARAMETER_INDEX_RDMX).c_str());
	m_optTemperature =			(float)atof(oneRow->at(CROP_PARAMETER_INDEX_T_OPT).c_str());
	m_minTemperature =			(float)atof(oneRow->at(CROP_PARAMETER_INDEX_T_BASE).c_str());
	m_yieldNFraction =			(float)atof(oneRow->at(CROP_PARAMETER_INDEX_CNYLD).c_str());
	m_yieldPFraction =			(float)atof(oneRow->at(CROP_PARAMETER_INDEX_CPYLD).c_str());
	m_biomassNFraction1 =			(float)atof(oneRow->at(CROP_PARAMETER_INDEX_BN1).c_str());
	m_biomassNFraction2 =			(float)atof(oneRow->at(CROP_PARAMETER_INDEX_BN2).c_str());
	m_biomassNFraction3 =			(float)atof(oneRow->at(CROP_PARAMETER_INDEX_BN3).c_str());
	m_biomassPFraction1 =			(float)atof(oneRow->at(CROP_PARAMETER_INDEX_BP1).c_str());
	m_biomassPFraction2 =			(float)atof(oneRow->at(CROP_PARAMETER_INDEX_BP2).c_str());
	m_biomassPFraction3 =			(float)atof(oneRow->at(CROP_PARAMETER_INDEX_BP3).c_str());
	m_harvestIndexLowerLimit =	(float)atof(oneRow->at(CROP_PARAMETER_INDEX_WSYF).c_str());
	m_minUSLEC =					(float)atof(oneRow->at(CROP_PARAMETER_INDEX_USLE_C).c_str());
	m_maxStomatalConductance =	(float)atof(oneRow->at(CROP_PARAMETER_INDEX_GSI).c_str());
	m_secondVPD =					(float)atof(oneRow->at(CROP_PARAMETER_INDEX_VPDFR).c_str());
	m_secondStomatalConductance =	(float)atof(oneRow->at(CROP_PARAMETER_INDEX_FRGMAX).c_str());
	m_radiationUseEfficiencyDeclineRateWithVPD = (float)atof(oneRow->at(CROP_PARAMETER_INDEX_WAVP).c_str());
	m_secondCO2Concentration =	(float)atof(oneRow->at(CROP_PARAMETER_INDEX_CO2HI).c_str());
	m_secondBiomassEnergyRatio =	(float)atof(oneRow->at(CROP_PARAMETER_INDEX_BIOEHI).c_str());
	m_plantResidueDecompositionCoefficient = (float)atof(oneRow->at(CROP_PARAMETER_INDEX_RSDCO_PL).c_str());
	m_defaultManningN =			(float)atof(oneRow->at(CROP_PARAMETER_INDEX_OV_N).c_str());
	m_CN2A =						(float)atof(oneRow->at(CROP_PARAMETER_INDEX_CN2A).c_str());
	m_CN2B =						(float)atof(oneRow->at(CROP_PARAMETER_INDEX_CN2B).c_str());
	m_CN2C =						(float)atof(oneRow->at(CROP_PARAMETER_INDEX_CN2C).c_str());
	m_CN2D =						(float)atof(oneRow->at(CROP_PARAMETER_INDEX_CN2D).c_str());
	m_isAutoFertilizer =			(atoi(oneRow->at(CROP_PARAMETER_INDEX_FERTFIELD).c_str()) != 0 ? true : false);
	m_minDormantLAI =				(float)atof(oneRow->at(CROP_PARAMETER_INDEX_ALAI_MIN).c_str());
	m_treeDormantResidueBiomassFraction = (float)atof(oneRow->at(CROP_PARAMETER_INDEX_BIO_LEAF).c_str());
}


CropParameter::~CropParameter(void)
{
}

void CropParameter::Dump(ostream* fs)
{
	if(fs == NULL) return;

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
	*fs << "FRGMAX:" << m_secondStomatalConductance  << endl;
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
