#include "TillageParameter.h"
#include "stdlib.h"

using namespace MainBMP;
using namespace BMPParameter;

#define TILLAGE_PARAM_COUNT                7
/// ITNUM, EFTMIX, DEPTIL, RRNS, CNOP_CN2, PRC, DSC
#define TILLAGE_PARAM_EFFMIX_IDX        1
#define TILLAGE_PARAM_DEPTIL_IDX        2
#define TILLAGE_PARAM_RRNS_IDX            3
#define TILLAGE_PARAM_CNOP_IDX            4
#define TILLAGE_PARAM_PRC_IDX            5
#define TILLAGE_PARAM_DSC_IDX            6

TillageParameter::TillageParameter(vector<string> *oneRow) : ArealParameter(oneRow)
{
    if (int(oneRow->size()) < TILLAGE_PARAM_COUNT)
        throw ModelException("TillageParameter", "TillageParameter", "oneRow should have 5 parameters at least!");
    this->m_mixingEfficiency = (float) atof(oneRow->at(TILLAGE_PARAM_EFFMIX_IDX).c_str());
    this->m_mixingDepth = (float) atof(oneRow->at(TILLAGE_PARAM_DEPTIL_IDX).c_str());
}

TillageParameter::TillageParameter(float *tillageFlds, int count) : ArealParameter((int) tillageFlds[0])
{
    if (count != TILLAGE_PARAM_COUNT)
        throw ModelException("TillageParameter", "TillageParameter",
                             "The size of tillage parameter fields should be 7!");
    this->m_mixingEfficiency = tillageFlds[TILLAGE_PARAM_EFFMIX_IDX];
    this->m_mixingDepth = tillageFlds[TILLAGE_PARAM_DEPTIL_IDX];
}

TillageParameter::~TillageParameter(void)
{
}

void TillageParameter::Dump(ostream *fs)
{
    if (fs == NULL) return;
    ArealParameter::Dump(fs);
    *fs << "EFFMIX:" << this->m_mixingEfficiency << endl;
    *fs << "DEPTIL:" << this->m_mixingDepth << endl;
}
