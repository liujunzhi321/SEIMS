#pragma once

#include <vector>
#include <map>
#include <string>
#include <cmath>

using namespace std;
/*!
 * \class Subbasin
 * \ingroup base
 * \brief Subbasin related parameters and methods.
 * Usage:
 *       1. Subbasin()
 *       2. setParameter()
 *       3. setCellList()
 *       4. setSoilLayers()
 *       5. setInputs()
 *       6. Get functions
 * \Revision:   1. Remove isOutput, since the ouput is handled in printInfo class
 *              2. 
 */
class Subbasin
{
public:
	/*
	 * \brief Constructor
	 * \param [in] id Subbasin ID
	 * \param [in] isOutput Is output defined by file.out or not
	 * \deprecated isOutput flag is useless here. By LJ
	 */
    Subbasin(int id, bool isOutput);
	/*
	 * \brief Constructor
	 * \param [in] id Subbasin ID
	 */
    Subbasin(int id);
	//! Destructor
    ~Subbasin(void);

private:
    //! Subbasin ID
    int m_id;

    //! valid cells number
    int m_nCells;
	//! index of valid cells
    int *m_cells;
	//! soil layers number of valid cells
	int *m_nSoilLayers;

    //inputs

	//! PET
    float m_PET;
    //float m_EI;
    //float m_ED;
    //float m_ES;
    
	//! average percolation (mm) of each valid cells 
    float m_PERCO;
	//! area of one cell
    float m_cellArea;    
	//! area of current Subbasin
    float m_Area; 

    //parameters
    
    //float m_GW0;

	//! maximum groundwater storage
    float m_GWMAX;
	//! baseflow recession coefficient
    float m_kg;
	//! groundwater Revap coefficient
    float m_dp_co;
	//! baseflow recession exponent
    float m_base_ex;
	//! 
    float m_QGCoefficient;
	//! slope correction factor of current subbasin
    float m_slopeCoefficient;
	//! average slope of the subbasin
    float m_slope;
	//! 
    float m_EG;
	//! initial groundwater or time (t-1)
    float m_GW;
	//! 
    float m_PERDE;
	//! 
    float m_QG;
	//! 
    float m_RG;
	//! Is output defined by file.out or not
    bool m_isOutput; 
	//! Is the revap (m_EG) is different from last time step
    bool m_isRevapChanged;
public:
	//! Check input size
	bool CheckInputSize(int n);
	//! Set cell index list, as well as subbasin area
    void setCellList(int nCells, int *cells);
	//! Set soil layers number of valid cells
	void setSoilLayers(int nCells, int *soilLayers);
	//! Set parameters of current subbasin
    void setParameter(float rv_co, float GW0, float GWMAX, float kg, float base_ex, float cellWidth, int timeStep);
	//! Set slope of current subbasin as the average of all cells
    void setSlope(float *slope);
	//! Set slope correction factor of current subbasin
    void setSlopeCoefofBasin(float slopeBasin);
	//! 
    void setInputs(float *PET, float *EI, float *ED, float *ES, float **PERCO, float groundwaterFromBankStorage);
	// Get functions
	
	//! Get subbasin ID
	int getId(){return m_id;};
	//! Get valid cells number
	int getCellCount(){return m_nCells;};
	//! Get index of valid cells
	int *getCells(){return m_cells;};
	//! Get soil layers number of valid cells
	int *getSoilLayers(){return m_nSoilLayers;};
	//! Get the output flag (true mean output), the function will be deprecated. By LJ
	bool getIsOutput(){return m_isOutput;};
	//! Get the Revap change flat (true mean changed from last time step)
	bool getIsRevapChanged(){return m_isRevapChanged;};
	//! Get average percolation (mm)
	float getPerco(){return m_PERCO;};
	//! Get
	float getPerde(){return m_PERDE;};
	//! Get average slope (%)
	float getSlope(){return m_slope;};
	//! 
	float getEG(){return m_EG;};
	//! Get groundwater ()
	float getGW(){return m_GW;};
	//! Get 
	float getRG(){return m_RG;};
	//! Get 
	float getQG(){return m_QG;};
	//! Get average PET
	float getPET(){return m_PET;};
	/*
	 * \brief Get basin (watershed) scale variable (key) value
	 * \param [in] key Variable name which is defined in text.h
	 * \param [in] *subbasins Vector of all Subbasins
	 */
    static float subbasin2basin(string key, vector<Subbasin *> *subbasins);
};

