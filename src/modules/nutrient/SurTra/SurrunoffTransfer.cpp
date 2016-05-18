/*!
 * \file NandPmi.cpp
 * \ingroup SurTra
 * \author Huiran Gao
 * \date May 2016
 */

#include <iostream>
#include "SurrunoffTransfer.h"
#include "MetadataInfo.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include "ModelException.h"
#include "util.h"
#include <omp.h>
using namespace std;

SurrunoffTransfer::SurrunoffTransfer(void):
//input 
m_nCells(-1), m_cellWidth(-1), m_nLayers(3), m_sedimentYield(NULL), m_surfr(NULL), m_sol_bd(NULL), m_sol_z(NULL),
	m_sol_actp(NULL), m_sol_orgn(NULL), m_sol_orgp(NULL), m_sol_stap(NULL), m_sol_aorgn(NULL), m_sol_fon(NULL), m_sol_fop(NULL),
	//output 
	m_sedorgn(NULL), m_sedorgp(NULL), m_sedminpa(NULL), m_sedminps(NULL)
{

}

SurrunoffTransfer::~SurrunoffTransfer(void) {
	///TODO
}
bool SurrunoffTransfer::CheckInputSize(const char* key, int n) {
	if(n <= 0) {
		//StatusMsg("Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(m_nCells != n) {
		if(m_nCells <=0) {
			m_nCells = n;
		} else {
			//StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
			ostringstream oss;
			oss << "Input data for "+string(key) << " is invalid with size: " << n << ". The origin size is " << m_nCells << ".\n";  
			throw ModelException("Denitrification","CheckInputSize",oss.str());
		}
	}
	return true;
}
bool SurrunoffTransfer::CheckInputData() {
	if(this->m_nCells == -1) {
		throw ModelException("SurTra","CheckInputData","You have not set the date time.");
		return false;
	}
	if(m_nCells <= 0) {
		throw ModelException("SurTra","CheckInputData","The dimension of the input data can not be less than zero.");
		return false;
	}
	if(this->m_cellWidth == NULL) {
		throw ModelException("SurTra","CheckInputData","The relative humidity can not be NULL.");
		return false;
	}
	if(this->m_sedimentYield == NULL) {
		throw ModelException("SurTra","CheckInputData","The solar radiation can not be NULL.");
		return false;
	}
	if(this->m_surfr == NULL) {
		throw ModelException("SurTra","CheckInputData","The min temperature can not be NULL.");
		return false;
	}

	///...
	return true;
}
void SurrunoffTransfer::SetValue(const char* key, float value)
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
		throw ModelException("SurTra","SetValue","Parameter " + sk + " does not exist in CLIMATE method. Please contact the module developer.");
	}
}
void SurrunoffTransfer::Set1DData(const char* key,int n, float *data)
{
	if(!this->CheckInputSize(key,n)) return;

	string sk(key);
	if (StringMatch(sk, VAR_CDN)) {
		this -> m_sedimentYield = data;
	} 
	else if (StringMatch(sk, VAR_LCC)) {
		this -> m_surfr = data;
	}
	else {
		throw ModelException("SurTra","SetValue","Parameter " + sk + " does not exist in CLIMATE module. Please contact the module developer.");
	}
}
void SurrunoffTransfer::Set2DData(const char* key, int nRows, int nCols, float** data)
{
	if(!this->CheckInputSize(key,nCols)) return;

	string sk(key);
	if (StringMatch(sk, VAR_ROOTDEPTH)) {
		this -> m_sol_z = data;
	}
	else if (StringMatch(sk, VAR_SOL_BD)) {
		this -> m_sol_bd = data;
	}
	else if (StringMatch(sk, VAR_SOL_AORGN)) {
		this -> m_sol_aorgn = data;
	}
	else if (StringMatch(sk, VAR_SOL_ORGN)) {
		this -> m_sol_orgn = data;
	}
	else if (StringMatch(sk, VAR_SOL_ORGP)) {
		this -> m_sol_orgp = data;
	}
	else if (StringMatch(sk, VAR_SOL_FOP)) {
		this -> m_sol_fop = data;
	}
	else if (StringMatch(sk, VAR_SOL_FON)) {
		this -> m_sol_fon = data;
	}
	else if (StringMatch(sk, VAR_SOL_ACTP)) {
		this -> m_sol_actp = data;
	}
	else if (StringMatch(sk, VAR_SOL_STAP)) {
		this -> m_sol_stap = data;
	}
	else if (StringMatch(sk, VAR_SOL_MP)) {
		this -> m_sol_mp = data;
	}
	else {
		throw ModelException("SurTra","SetValue","Parameter " + sk + " does not exist in CLIMATE module. Please contact the module developer.");
	}
}
int SurrunoffTransfer::Execute() {
	if(!this -> CheckInputData()) { 
		return false;
	}
	//Calculate daily nitrogen and phosphorus mineralization and immobilization.
	//CalculateEnrRatio();
	//Calculates the amount of organic nitrogen removed in surface runoff.
	OrgnRemoveinSr();
	//Calculates the amount of organic and mineral phosphorus attached to sediment in surface runoff.
	OrgpAttachedtoSed();

	//return ??
	return 0;
}
float* SurrunoffTransfer::CalculateEnrRatio(){
	//enrichment ratio
	float* enratio;
	for(int i = 0; i < m_nCells; i++) {
		if (m_sedimentYield[i] < 1e-4) {
			m_sedimentYield[i] = 0;
		}
		// CREAMS method for calculating enrichment ratio
		float cy = 0;
		// Calculate sediment calculations, equation 4:2.2.3 in SWAT Theory 2009, p272
		cy = 0.1 * m_sedimentYield[i] / (m_cellWidth * m_cellWidth * 0.0001 * m_surfr[i] + 1e-6);
		if (cy > 1e-6) {
			enratio[i] = 0.78 * pow(cy, -0.2468f);
		} else {
			enratio[i] = 0;
		}
		if (enratio[i] > 3.5) {
			enratio[i] = 3.5;
		}
	}
	return enratio;
}
void SurrunoffTransfer::OrgnRemoveinSr(){
	//enrichment ratio (enratio)
	float* enratio = CalculateEnrRatio();
	for(int i = 0; i < m_nCells; i++) {
		for(int k = 0; k < m_nLayers; k++) {
			//amount of organic N in first soil layer (orgninfl)
			float orgninfl = 0;
			//conversion factor (wt)
			float wt = 0;
			orgninfl = m_sol_orgn[i][0] + m_sol_aorgn[i][0] + m_sol_fon[i][0];
			wt = m_sol_bd[i][0] * m_sol_z[i][0] / 100;
			//concentration of organic N in soil (concn)
			float concn = 0;
			concn = orgninfl * enratio[i] / wt;
			//Calculate the amount of organic nitrogen transported with sediment to the stream, equation 4:2.2.1 in SWAT Theory 2009, p271
			m_sedorgn[i] = 0.001 * concn * m_sedimentYield[i] / (m_cellWidth * m_cellWidth * m_nCells);
			//update soil nitrogen pools
			if(orgninfl > 1e-6) {
				m_sol_aorgn[i][0] = m_sol_aorgn[i][0] - m_sedorgn[i] * (m_sol_aorgn[i][0] / orgninfl);
				m_sol_orgn[i][0] = m_sol_orgn[i][0] - m_sedorgn[i] * (m_sol_orgn[i][0] / orgninfl);
				m_sol_fon[i][0] = m_sol_fon[i][0] - m_sedorgn[i] * (m_sol_fon[i][0] / orgninfl);
				if (m_sol_aorgn[i][0] < 0) {
					m_sedorgn[i] = m_sedorgn[i] + m_sol_aorgn[i][0];
					m_sol_aorgn[i][0] = 0;
				}
				if (m_sol_orgn[i][0] < 0) {
					m_sedorgn[i] = m_sedorgn[i] + m_sol_orgn[i][0];
					m_sol_orgn[i][0] = 0;
				}
				if (m_sol_fon[i][0] < 0.) {
					m_sedorgn[i] = m_sedorgn[i] + m_sol_fon[i][0];
					m_sol_fon[i][0] = 0;
				}
			}

		}
	}
}
void SurrunoffTransfer::OrgpAttachedtoSed(){
	float* enratio = CalculateEnrRatio();
	for(int i = 0; i < m_nCells; i++) {
		for(int k = 0; k < m_nLayers; k++) {
			//amount of phosphorus attached to sediment in soil (sol_attp)
			float sol_attp = 0;
			//fraction of active mineral/organic/stable mineral phosphorus in soil (sol_attp_o, sol_attp_a, sol_attp_s)
			float sol_attp_o = 0;
			float sol_attp_a = 0;
			float sol_attp_s = 0;
			//Calculate sediment
			sol_attp = m_sol_orgp[i][0] + m_sol_fop[i][0] + m_sol_mp[i][0] + m_sol_actp[i][0] + m_sol_stap[i][0];
			if (sol_attp > 1e-3) {
				sol_attp_o = (m_sol_orgp[i][0] + m_sol_fop[i][0]+ m_sol_mp[i][0]) / sol_attp;
				sol_attp_a = m_sol_actp[i][0] / sol_attp;
				sol_attp_s = m_sol_stap[i][0] / sol_attp;
			}
			//conversion factor (mg/kg => kg/ha) (wt)
			float wt = m_sol_bd[i][0] * m_sol_z[i][0] / 100;
			//concentration of organic P in soil (concp)
			float concp = 0;
			concp = sol_attp * enratio[i] / wt;
			//total amount of P removed in sediment erosion (sedp)
			float sedp = 0.001 * concp * m_sedimentYield[i] / (m_cellWidth * m_cellWidth);
			m_sedorgp[i] = sedp * sol_attp_o;
			m_sedminpa[i] = sedp * sol_attp_a;
			m_sedminps[i] = sedp * sol_attp_s;
			//modify phosphorus pools
			//total amount of P in mineral sediment pools prior to sediment removal (psedd)
			float psedd = 0;
			//total amount of P in organic pools prior to sediment removal (porgg)
			float porgg = 0;
			psedd = m_sol_actp[i][0] + m_sol_stap[i][0];
			porgg = m_sol_orgp[i][0] + m_sol_fop[i][0];
			if (porgg > 1e-3) {
				m_sol_orgp[i][0] = m_sol_orgp[i][0] - m_sedorgp[i] * (m_sol_orgp[i][0] / porgg);
				m_sol_fop[i][0] = m_sol_fop[i][0] - m_sedorgp[i] * (m_sol_fop[i][0] / porgg);
				m_sol_mp[i][0] = m_sol_mp[i][0] - m_sedorgp[i] * (m_sol_mp[i][0] / porgg);
			}
			m_sol_actp[i][0] = m_sol_actp[i][0] - m_sedminpa[i];
			m_sol_stap[i][0] = m_sol_stap[i][0] - m_sedminps[i];
			if (m_sol_orgp[i][0] < 0) {
				m_sedorgp[i] = m_sedorgp[i] + m_sol_orgp[i][0];
				m_sol_orgp[i][0] = 0;
			}
			if (m_sol_fop[i][0] < 0) {
				m_sedorgp[i] = m_sedorgp[i] + m_sol_fop[i][0];
				m_sol_fop[i][0] = 0;
			}
			if (m_sol_mp[i][0] < 0) {
				m_sedorgp[i] = m_sedorgp[i] + m_sol_mp[i][0];
				m_sol_mp[i][0] = 0;
			}
			if (m_sol_actp[i][0] < 0) {
				m_sedminpa[i] = m_sedminpa[i] + m_sol_actp[i][0];
				m_sol_actp[i][0] = 0;
			}
			if (m_sol_stap[i][0] < 0) {
				m_sedminps[i] = m_sedminps[i] + m_sol_stap[i][0];
				m_sol_stap[i][0] = 0;
			}

		}
	}
}
void SurrunoffTransfer::Get1DData(const char* key, int* n, float** data) {
	string sk(key);
	*n = m_nCells;
	if (StringMatch(sk, VAR_SEDORGN)) {
		*data = this -> m_sedorgn;
	}
	if (StringMatch(sk, VAR_SEDORGP)) {
		*data = this -> m_sedorgp;
	}
	if (StringMatch(sk, VAR_SEDMINPA)) {
		*data = this -> m_sedminpa;
	}
	if (StringMatch(sk, VAR_SEDMINPS)) {
		*data = this -> m_sedminps;
	}
	else {
		throw ModelException("SurTra", "GetValue","Parameter " + sk + " does not exist. Please contact the module developer.");
	}
}
void SurrunoffTransfer::Get2DData(const char* key, int *nRows, int *nCols, float*** data) {
	string sk(key);
	*nRows = m_nLayers;
	*nCols = m_nCells;
	if (StringMatch(sk, VAR_SOL_AORGN)) {
		*data = this -> m_sol_aorgn; 
	}
	if (StringMatch(sk, VAR_SOL_FON)) {
		*data = this -> m_sol_fon; 
	}
	if (StringMatch(sk, VAR_SOL_ORGN)) {
		*data = this -> m_sol_orgn; 
	}
	if (StringMatch(sk, VAR_SOL_ORGP)) {
		*data = this -> m_sol_orgp; 
	}
	if (StringMatch(sk, VAR_SOL_FOP)) {
		*data = this -> m_sol_fop; 
	}
	if (StringMatch(sk, VAR_SOL_STAP)) {
		*data = this -> m_sol_stap; 
	}
	if (StringMatch(sk, VAR_SOL_ACTP)) {
		*data = this -> m_sol_actp; 
	}
	else
		throw ModelException("Denitrification", "Get2DData", "Output " + sk 
		+ " does not exist in the Denitrification module. Please contact the module developer.");
}
// int main() {
	//	system("pause");
//}