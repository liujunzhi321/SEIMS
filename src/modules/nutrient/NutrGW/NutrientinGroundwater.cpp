/*//
 * \file NutrientinGroundwater.cpp
 * \ingroup NutGW
 * \author Huiran Gao
 * \date Jun 2016
 */

#include <iostream>
#include "NutrientinGroundwater.h"
#include "MetadataInfo.h"
#include <cmath>
#include <fstream>
#include "ModelException.h"
#include "util.h"
#include <omp.h>
using namespace std;

NutrientinGroundwater::NutrientinGroundwater(void):
	//input 
	m_nCells(-1), m_cellWidth(-1), m_gwno3(NULL), m_gwminp(NULL), m_gw_q(NULL),
	//output 
	m_minpgw(NULL), m_no3gw(NULL)
{

}

NutrientinGroundwater::~NutrientinGroundwater(void) {
	///TODO
}
bool NutrientinGroundwater::CheckInputSize(const char* key, int n) {
	if(n <= 0) {
		throw ModelException(MID_MINRL, "CheckInputSize", "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
		return false;
	}
	if(m_nCells != n) {
		if(m_nCells <=0) {
			m_nCells = n;
		} else {
			//StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
			ostringstream oss;
			oss << "Input data for "+string(key) << " is invalid with size: " << n << ". The origin size is " << m_nCells << ".\n";  
			throw ModelException("NutGW","CheckInputSize",oss.str());
		}
	}
	return true;
}
bool NutrientinGroundwater::CheckInputData() {
	if(m_nCells <= 0) {throw ModelException("NutGW", "CheckInputData", "The input data can not be less than zero.");return false;}
	if(this->m_cellWidth < 0) {throw ModelException("NutGW", "CheckInputData", "The input data can not be NULL.");return false;}
	if(this->m_gwno3 == NULL) {throw ModelException("NutGW", "CheckInputData", "The input data can not be NULL.");return false;}
	if(this->m_gwminp == NULL) {throw ModelException("NutGW", "CheckInputData", "The input data can not be NULL.");return false;}
	if(this->m_gw_q == NULL) {throw ModelException("NutGW", "CheckInputData", "The input data can not be NULL.");return false;}
	return true;
}
void NutrientinGroundwater::SetValue(const char* key, float value)
{
	string sk(key);
	if (StringMatch(sk, VAR_OMP_THREADNUM)) {
		omp_set_num_threads((int)value);
	} 
	else if (StringMatch(sk, Tag_CellSize)) {
		this -> m_nCells = value;
	}
	else if (StringMatch(sk, Tag_CellWidth)) {
		this -> m_cellWidth = value;
	}
	else {
		throw ModelException("NutGW","SetValue","Parameter " + sk + " does not exist in CLIMATE method. Please contact the module developer.");
	}
}
void NutrientinGroundwater::Set1DData(const char* key,int n, float *data)
{
	if(!this->CheckInputSize(key,n)) return;

	string sk(key);
	if (StringMatch(sk, VAR_GWNO3)) {
		this -> m_gwno3 = data;
	}
	else if (StringMatch(sk, VAR_GWMINP)) {
		this -> m_gwminp = data;
	}
	else if (StringMatch(sk, VAR_GW_Q)) {
		this -> m_gw_q = data;
	}
	else {
		throw ModelException("NutGW","SetValue","Parameter " + sk + " does not exist in CLIMATE module. Please contact the module developer.");
	}
}
void NutrientinGroundwater::initialOutputs() {
	if(this->m_nCells <= 0) {
		throw ModelException("AtmosphericDeposition", "CheckInputData", "The dimension of the input data can not be less than zero.");
	}
	// allocate the output variables
	if(m_no3gw == NULL) {
		for(int i=0; i < m_nCells; i++) {
			m_no3gw[i] = 0.f;
			m_minpgw[i] = 0.f;
		}
	}
}
int NutrientinGroundwater::Execute() {
	if(!this -> CheckInputData()) { 
		return false;
	}
	this -> initialOutputs();
	for(int i = 0; i < m_nCells; i++) {

		m_no3gw[i] = m_gwno3[i] * m_gw_q[i] / 100.f;
		m_minpgw[i] = m_gwminp[i] * m_gw_q[i] / 100.f;

	}
	//return ??
	return 0;
}
void NutrientinGroundwater::Get1DData(const char* key, int* n, float** data) {
	string sk(key);
	*n = m_nCells;
	if (StringMatch(sk, VAR_NO3GW)) {
		*data = this -> m_no3gw;
	}
	if (StringMatch(sk, VAR_MINPGW)) {
		*data = this -> m_minpgw;
	}
	else {
		throw ModelException("NutGW", "GetValue","Parameter " + sk + " does not exist. Please contact the module developer.");
	}
}
