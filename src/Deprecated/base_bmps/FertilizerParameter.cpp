#include "FertilizerParameter.h"

using namespace MainBMP;
using namespace BMPParameter;

#define FERTILIZER_PARAM_COUNT 10
/// IFNUM, FMINN, FMINP, FORGN, FORGP, FNH3N, BACTPDB, BACTLPDB, BACTKDDB, MANURE
#define    FERTILIZER_PARAM_FMINN_IDX                1
#define    FERTILIZER_PARAM_FMINP_IDX                2
#define    FERTILIZER_PARAM_FORGN_IDX                3
#define    FERTILIZER_PARAM_FORGP_IDX                4
#define    FERTILIZER_PARAM_FNH3N_IDX                5
#define    FERTILIZER_PARAM_BACTPDB_IDX            6
#define    FERTILIZER_PARAM_BATTLPDB_IDX        7
#define    FERTILIZER_PARAM_BACKTKDDB_IDX    8

FertilizerParameter::FertilizerParameter(vector<string> *oneRow) : ArealParameter(oneRow)
{
    if (int(oneRow->size()) < FERTILIZER_PARAM_COUNT)
        throw ModelException("FertilizerParameter", "FertilizerParameter",
                             "oneRow should have 11 parameters at least!");

    this->m_mineralNFraction = (float) atof(oneRow->at(FERTILIZER_PARAM_FMINN_IDX).c_str());
    this->m_mineralPFraction = (float) atof(oneRow->at(FERTILIZER_PARAM_FMINP_IDX).c_str());
    this->m_organicNFraction = (float) atof(oneRow->at(FERTILIZER_PARAM_FORGN_IDX).c_str());
    this->m_organicPFraction = (float) atof(oneRow->at(FERTILIZER_PARAM_FORGP_IDX).c_str());
    this->m_NH3Fraction = (float) atof(oneRow->at(FERTILIZER_PARAM_FNH3N_IDX).c_str());
    this->m_persistentBacteriaFraction = (float) atof(oneRow->at(FERTILIZER_PARAM_BACTPDB_IDX).c_str());
    this->m_nonpersistentBacteriaFraction = (float) atof(oneRow->at(FERTILIZER_PARAM_BATTLPDB_IDX).c_str());
    this->m_bacteriaPartitionCoefficient = (float) atof(oneRow->at(FERTILIZER_PARAM_BACKTKDDB_IDX).c_str());
}

FertilizerParameter::FertilizerParameter(float *fertilizerFlds, int count) : ArealParameter((int) fertilizerFlds[0])
{
    if (count != FERTILIZER_PARAM_COUNT)
        throw ModelException("FertilizerParameter", "FertilizerParameter",
                             "Fertilizer should have 10 numeric parameters!");

    this->m_mineralNFraction = fertilizerFlds[FERTILIZER_PARAM_FMINN_IDX];
    this->m_mineralPFraction = fertilizerFlds[FERTILIZER_PARAM_FMINP_IDX];
    this->m_organicNFraction = fertilizerFlds[FERTILIZER_PARAM_FORGN_IDX];
    this->m_organicPFraction = fertilizerFlds[FERTILIZER_PARAM_FORGP_IDX];
    this->m_NH3Fraction = fertilizerFlds[FERTILIZER_PARAM_FNH3N_IDX];
    this->m_persistentBacteriaFraction = fertilizerFlds[FERTILIZER_PARAM_BACTPDB_IDX];
    this->m_nonpersistentBacteriaFraction = fertilizerFlds[FERTILIZER_PARAM_BATTLPDB_IDX];
    this->m_bacteriaPartitionCoefficient = fertilizerFlds[FERTILIZER_PARAM_BACKTKDDB_IDX];
}

FertilizerParameter::~FertilizerParameter(void)
{
}

void FertilizerParameter::Dump(ostream *fs)
{
    if (fs == NULL) return;
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
