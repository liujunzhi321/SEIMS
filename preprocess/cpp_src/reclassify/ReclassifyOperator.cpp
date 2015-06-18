#include "ReclassifyOperator.h"
#include <iostream>

using namespace std;

void ReclassifyOperator::SetTypeLayer(RasterLayer<float> &layerD) 
{
  _pTypeLayer = &layerD;
  _pNrhood = layerD.nbrhood();
  cellSize = _pTypeLayer->_pMetaData->cellSize;
  noData = _pTypeLayer->_pMetaData->noData;
    //pWorkBR = &_pTypeLayer->_pMetaData->_localworkBR;
  Configure(_pTypeLayer, false);
}

void ReclassifyOperator::SetOutputLayer(RasterLayer<float> &layerD) 
{
  _pOutputLayer = &layerD;
  Configure(_pOutputLayer,false);
}

bool ReclassifyOperator::ReadReclassMap(const char* filename)
{
    ifstream ofs(filename);
    int n;
    ofs >> n;
    int k;
    float value;
    for(int i = 0; i < n; i++)
    {
        ofs >> k >> value;
        _reclassMap[k] = value;
    }
    return true;
}


bool ReclassifyOperator::isTermination()
{
	num--;
	if(num > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ReclassifyOperator::Operator(const CellCoord &coord)
{
	CellSpace<float> &typeValue = *(_pTypeLayer->cellSpace());
	
	CellSpace<float> &output = *(_pOutputLayer->cellSpace());
	  
	int iRow = coord.iRow();
	int iCol = coord.iCol();
	
    int k = int(typeValue[iRow][iCol]);
    map<int, float>::iterator iter = _reclassMap.find(k);
    if (iter != _reclassMap.end())
        output[iRow][iCol] = iter->second;
    else
        output[iRow][iCol] = (float)noData; 

	return true;
}

