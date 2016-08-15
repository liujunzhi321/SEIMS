#include "api.h"
#include <iostream>
#include "NonPointSource_Management.h"
#include "MetadataInfo.h"
#include "ModelException.h"

using namespace std;

NPS_Management::NPS_Management(void) : m_nCells(-1)
{
}


NPS_Management::~NPS_Management(void)
{
}

void NPS_Management::Set1DData(const char *key, int n, float *data)
{
}

void NPS_Management::Set2DData(const char *key, int n, int col, float **data)
{
}

int NPS_Management::Execute()
{
    return true;
}

void NPS_Management::Get1DData(const char *key, int *n, float **data)
{
}

void NPS_Management::Get2DData(const char *key, int *n, int *col, float ***data)
{
}