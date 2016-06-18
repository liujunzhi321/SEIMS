#include "api.h"
#include <iostream>
#include "managementOperation_SWAT.h"
#include "MetadataInfo.h"
#include "ModelException.h"
using namespace std;
MGTOpt_SWAT::MGTOpt_SWAT(void):m_nCells(-1)
{
}


MGTOpt_SWAT::~MGTOpt_SWAT(void)
{
}

void MGTOpt_SWAT::Set1DData(const char* key, int n, float* data)
{
}

void MGTOpt_SWAT::Set2DData(const char* key, int n, int col, float** data)
{
}
int MGTOpt_SWAT::Execute()
{
	return true;
}

void MGTOpt_SWAT::Get1DData(const char* key, int* n, float** data)
{
}
void MGTOpt_SWAT::Get2DData(const char* key, int* n, int* col, float*** data)
{
}