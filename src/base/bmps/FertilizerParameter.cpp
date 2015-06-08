#include "FertilizerParameter.h"

using namespace MainBMP;
using namespace BMPParameter;

#define FERTILIZER_PARAMETER_COUNT 11

#define 	FERTILIZER_PARAMETER_INDEX_FMINN		3
#define 	FERTILIZER_PARAMETER_INDEX_FMINP		4
#define 	FERTILIZER_PARAMETER_INDEX_FORGN		5
#define 	FERTILIZER_PARAMETER_INDEX_FORGP		6
#define 	FERTILIZER_PARAMETER_INDEX_FNH3N		7
#define 	FERTILIZER_PARAMETER_INDEX_BACTPDB		8
#define 	FERTILIZER_PARAMETER_INDEX_BATTLPDB		9
#define 	FERTILIZER_PARAMETER_INDEX_BACKTKDDB	10

FertilizerParameter::FertilizerParameter(vector<string> *oneRow):ArealParameter(oneRow)
{
	if(int(oneRow->size()) < FERTILIZER_PARAMETER_COUNT)	throw ModelException("FertilizerParameter","FertilizerParameter","oneRow should have 11 parameters at least!");
	
	this->m_mineralNFraction =	(float)atof(oneRow->at(FERTILIZER_PARAMETER_INDEX_FMINN).c_str());
	this->m_mineralPFraction =	(float)atof(oneRow->at(FERTILIZER_PARAMETER_INDEX_FMINP).c_str());
	this->m_organicNFraction =	(float)atof(oneRow->at(FERTILIZER_PARAMETER_INDEX_FORGN).c_str());
	this->m_organicPFraction =	(float)atof(oneRow->at(FERTILIZER_PARAMETER_INDEX_FORGP).c_str());
	this->m_NH3Fraction		 =  (float)atof(oneRow->at(FERTILIZER_PARAMETER_INDEX_FNH3N).c_str());
	this->m_persistentBacteriaFraction = (float)atof(oneRow->at(FERTILIZER_PARAMETER_INDEX_BACTPDB).c_str());
	this->m_nonpersistentBacteriaFraction = (float)atof(oneRow->at(FERTILIZER_PARAMETER_INDEX_BATTLPDB).c_str());
	this->m_bacteriaPartitionCoefficient = (float)atof(oneRow->at(FERTILIZER_PARAMETER_INDEX_BACKTKDDB).c_str());
}


FertilizerParameter::~FertilizerParameter(void)
{
}

void FertilizerParameter::Dump(ostream* fs)
{
	if(fs == NULL) return;

	ArealParameter::Dump(fs);

	*fs << "FMINN:" << this->m_mineralNFraction << endl;
	*fs << "FMINP:" << this->m_mineralPFraction << endl;
	*fs << "FORGN:" << this->m_organicNFraction << endl;
	*fs << "FORGP:" << this->m_organicPFraction << endl;
	*fs << "FNH3N:" << this->m_NH3Fraction << endl;
	*fs << "BACTPDB:" << this->m_persistentBacteriaFraction << endl;
	*fs << "BACTLPDB:" << this->m_nonpersistentBacteriaFraction << endl;
	*fs << "BACKTKDDB:" << this->m_bacteriaPartitionCoefficient << endl;
}
