/*!
 * \file AtmosphericDeposition.cpp
 * \ingroup ATMDEP
 * \author Huiran Gao
 * \date April 2016
 */
#include "AtmosphericDeposition.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>
#include <cmath>
#include <iostream>

using namespace std;

/// In nutrient modules, m_nSoilLayers is 3, 0~10mm, 10~100mm, and 100~rootDepth. 
/// In SEIMS, m_nSoilLayers is 2, 0~100mm, 100~rootDepth.
AtmosphericDeposition::AtmosphericDeposition(void):
	//input
	m_nCells(-1), m_cellWidth(-1), m_rcn(-1.0f), m_rca(-1.0f), 
	m_nSoilLayers(NULL), m_sol_z(NULL), m_preci(NULL), m_drydep_no3(NULL), m_drydep_nh4(NULL),
	//output
	m_sol_no3(NULL), m_sol_nh3(NULL), m_addrno3(NULL), m_addrnh3(NULL), m_wshd_rno3(NULL)
{

}

AtmosphericDeposition::~AtmosphericDeposition(void){
	
}

bool AtmosphericDeposition::CheckInputData(void) {
	if(m_nCells <= 0) {throw ModelException("AtmosphericDeposition","CheckInputData","The cell number of the input can not be less than zero.");return false;}
	if(this -> m_soiLayers < 0) {throw ModelException("NmiRL", "CheckInputData", "The input data can not be NULL.");return false;}
	if(this -> m_cellWidth < 0) {throw ModelException("NminRL", "CheckInputData", "The data can not be NULL.");return false;}
	if(this -> m_nSoilLayers == NULL) {throw ModelException("NminRL", "CheckInputData", "The data can not be NULL.");return false;}
	if(this -> m_preci == NULL) {throw ModelException("AtmosphericDeposition","CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_rcn < 0) {throw ModelException("AtmosphericDeposition","CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_rca < 0) {throw ModelException("AtmosphericDeposition","CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_sol_z == NULL) {throw ModelException("AtmosphericDeposition","CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_drydep_no3 == NULL) {throw ModelException("AtmosphericDeposition","CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_drydep_nh4 == NULL) {throw ModelException("AtmosphericDeposition","CheckInputData","The input data can not be NULL.");return false;}
	return true;
}
bool AtmosphericDeposition::CheckInputSize(const char* key, int n){
	if(n <= 0) {
		throw ModelException(MID_ATMDEP, "CheckInputSize", "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
		return false;
	}
	if(m_nCells != n) {
		if(m_nCells <=0) {
			m_nCells = n;
		} else {
			//StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
			ostringstream oss;
			oss << "Input data for "+string(key) << " is invalid with size: " << n << ". The origin size is " << m_nCells << ".\n";  
			throw ModelException("AtmosphericDeposition","CheckInputSize",oss.str());
		}
	}

	return true;
}
void AtmosphericDeposition::Set1DData(const char* key, int n, float* data) {
	string sk(key);
	if(StringMatch(sk, VAR_PRECI)) {
		m_preci = data;
	}
	else {
		throw ModelException("CH_MSK", "Set1DData", "Parameter " + sk + " does not exist. Please contact the module developer.");
	}
}
void AtmosphericDeposition::SetValue(const char* key, float value) {
	string sk(key);
	if (StringMatch(sk, VAR_OMP_THREADNUM)) {
		omp_set_num_threads((int)value);
	}
	else if (StringMatch(sk, Tag_CellSize)) {m_nCells = value;}
	else if (StringMatch(sk, Tag_CellWidth)) {m_cellWidth = value;}
	else if (StringMatch(sk, VAR_RCN)) {m_rcn = value;}
	else if (StringMatch(sk, VAR_RCA)) {m_rca = value;}
	else if (StringMatch(sk, VAR_DRYDEP_NO3)) {m_drydep_no3 = value;}
	else if (StringMatch(sk, VAR_DRYDEP_NH4)) {m_drydep_nh4 = value;}
	else {
		throw ModelException("AtmosphericDeposition", "SetSingleData", "Parameter " + sk + " does not exist. Please contact the module developer.");
	}
}

void AtmosphericDeposition::Set2DData(const char* key, int nRows, int nCols, float** data) {
	if(!this->CheckInputSize(key,nCols)) return;
	string sk(key);
	m_soiLayers = nCols;
	if (StringMatch(sk, VAR_ROOTDEPTH)) {this -> m_sol_z = data;}
	else if (StringMatch(sk, VAR_SOL_NO3)) {this -> m_sol_no3 = data;}
	else if (StringMatch(sk, VAR_SOL_NH3)) {this -> m_sol_nh3 = data;}
	else {
		throw ModelException("SurTra","SetValue","Parameter " + sk + " does not exist in CLIMATE module. Please contact the module developer.");
	}
}
int AtmosphericDeposition::Execute() {
	//check the data
	CheckInputData();
	#pragma omp parallel for
	for(int i = 0; i < m_nCells; i++) {
		for(int k = 0; k < m_nSoilLayers[i]; k++) {
			// Calculate the amount of nitrite and ammonia added to the soil in rainfall, 
		    m_addrno3 = 0.01 * m_rcn * m_preci[i];
		    m_addrnh3 = 0.01 * m_rca * m_preci[i];
		    m_sol_no3[i][0] = m_sol_no3[i][0] + m_addrno3 + m_drydep_no3 * 365;
		    m_sol_nh3[i][0] = m_sol_nh3[i][0] + m_addrnh3 + m_drydep_nh4 * 365;

			m_wshd_rno3 = m_wshd_rno3 + m_addrno3 * (1 / m_cellWidth);
		}
	}
	return 0;
}
void AtmosphericDeposition::GetValue(const char* key, float* value) {
	string sk(key);
	if (StringMatch(sk, VAR_ADDRNO3)) {
		*value = m_addrno3;
	}
	else if(StringMatch(sk, VAR_WSHD_RNO3)) {
		*value = m_wshd_rno3;
	}
	else {
		throw ModelException("NMINRL", "GetValue","Parameter " + sk + " does not exist. Please contact the module developer.");
	}
}

void AtmosphericDeposition::Get2DData(const char* key, int *nRows, int *nCols, float*** data) {
	string sk(key);
	*nRows = m_nCells;
	*nCols = m_soiLayers;
	if (StringMatch(sk, VAR_SOL_NO3)) {
		*data = m_sol_no3;
	}
	else {
		throw ModelException("AtmosphericDeposition", "Get2DData", "Output " + sk + " does not exist in the Atmospheric Deposition module. Please contact the module developer.");
	}
}