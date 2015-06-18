#ifndef SLOPEOPERATOR_H
#define SLOPEOPERATOR_H

#include "cellSpace.h"
#include "neighborhood.h"
#include "rasterOperator.h"
#include "rasterLayer.h"
#include <cmath>
#include <functional>
#include <map>

using namespace GPRO;

class ReclassifyOperator : public RasterOperator<float> 
{
  public:
    ReclassifyOperator()
      :RasterOperator<float>(),
       _pTypeLayer(0), _pOutputLayer(0), num(0){}
   
    ~ReclassifyOperator() {}

  
    void SetTypeLayer(RasterLayer<float> &layerD);
	void SetOutputLayer(RasterLayer<float> &layerD);

	virtual bool isTermination();

    virtual bool Operator(const CellCoord &coord);

    bool ReadReclassMap(const char* filename);

	void SetDefualtType(int typeValue)
	{
		defaultType = typeValue;
	}

  protected:
	int cellSize;
	int noData;
	int num;
	int defaultType;
	RasterLayer<float> *_pTypeLayer;
	RasterLayer<float> *_pOutputLayer;
	Neighborhood<float> *_pNrhood;

    map<int, float> _reclassMap;
};

#endif
