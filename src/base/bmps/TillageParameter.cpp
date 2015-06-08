#include "TillageParameter.h"
#include "stdlib.h"

using namespace MainBMP;
using namespace BMPParameter;

#define TILLAGE_PARAMETER_COUNT 5

#define 	TILLAGE_PARAMETER_INDEX_EFFMIX		3
#define 	TILLAGE_PARAMETER_INDEX_DEPTIL		4

TillageParameter::TillageParameter(vector<string> *oneRow):ArealParameter(oneRow)
{
	if(int(oneRow->size()) < TILLAGE_PARAMETER_COUNT)	throw ModelException("TillageParameter","TillageParameter","oneRow should have 5 parameters at least!");
	
	this->m_mixingEfficiency	=	(float)atof(oneRow->at(TILLAGE_PARAMETER_INDEX_EFFMIX).c_str());
	this->m_mixingDepth			=	(float)atof(oneRow->at(TILLAGE_PARAMETER_INDEX_DEPTIL).c_str());
}


TillageParameter::~TillageParameter(void)
{
}

void TillageParameter::Dump(ostream* fs)
{
	if(fs == NULL) return;

	ArealParameter::Dump(fs);

	*fs << "EFFMIX:" << this->m_mixingEfficiency << endl;
	*fs << "DEPTIL:" << this->m_mixingDepth << endl;
}
