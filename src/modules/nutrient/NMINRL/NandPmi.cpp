/*!
 * \file NandPmi.cpp
 * \ingroup NminRL
 * \author Huiran Gao
 * \date April 2016
 */

#include <iostream>
#include "nandpmi.h"
#include "MetadataInfo.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include "ModelException.h"
#include "util.h"
#include <omp.h>
using namespace std;

NandPim::NandPim(void):
	//input 
	m_nCells(-1), m_cellWidth(-1), m_nLayers(3), m_cmn(NULL), m_cdn(NULL), m_idplt(NULL), m_nactfr(NULL), m_psp(NULL), m_sol_z(NULL),
	m_rsdco_pl(NULL), m_sol_cbn(NULL), m_sol_st(NULL), m_sol_tmp(NULL), m_sol_ul(NULL), m_sol_nh3(NULL), m_sol_fc(NULL), m_sol_wpmm(NULL),
	m_sol_actp(NULL), m_sol_stap(NULL), m_sol_aorgn(NULL), m_sol_fon(NULL), m_sol_fop(NULL), m_sol_no3(NULL),m_sol_orgn(NULL), m_sol_orgp(NULL),
	m_sol_rsd(NULL), m_sol_solp(NULL), m_wshd_dnit(NULL), m_wshd_hmn(0), m_wshd_hmp(NULL), m_wshd_rmn(NULL), m_wshd_rmp(NULL), m_wshd_rwn(NULL),
	m_wshd_nitn(NULL), m_wshd_voln(NULL), m_wshd_pal(NULL), m_wshd_pas(NULL),
	//output 
	m_hmntl(NULL), m_hmptl(NULL), m_rmn2tl(NULL), m_rmptl(NULL),m_rwntl(NULL), m_wdntl(NULL),m_rmp1tl(NULL), m_roctl(NULL) 
{

}

NandPim::~NandPim(void) {
	///TODO
}
bool NandPim::CheckInputSize(const char* key, int n) {
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
bool NandPim::CheckInputData() {
	if(this->m_nCells == -1) {
		throw ModelException("NminRL","CheckInputData","You have not set the date time.");
		return false;
	}
	if(m_nCells <= 0) {
		throw ModelException("NminRL","CheckInputData","The dimension of the input data can not be less than zero.");
		return false;
	}
	if(this->m_cellWidth == NULL) {
		throw ModelException("NminRL","CheckInputData","The relative humidity can not be NULL.");
		return false;
	}
	if(this->m_cmn == NULL) {
		throw ModelException("NminRL","CheckInputData","The solar radiation can not be NULL.");
		return false;
	}
	if(this->m_cdn == NULL) {
		throw ModelException("NminRL","CheckInputData","The min temperature can not be NULL.");
		return false;
	}
	if(this->m_idplt == NULL) {
		throw ModelException("NminRL","CheckInputData","The max temperature can not be NULL.");
		return false;
	}
	if(this->m_nactfr == NULL) {
		throw ModelException("NminRL","CheckInputData","The wind speed can not be NULL.");
		return false;
	}

	///...
	return true;
}
void NandPim::SetValue(const char* key, float value)
{
	string sk(key);
	if (StringMatch(sk, VAR_OMP_THREADNUM)) {
		omp_set_num_threads((int)value);
	} 
	else if (StringMatch(sk, Tag_CellSize)) {
		this -> m_nCells = (int)value;
	} 
	else if (StringMatch(sk, Tag_CellWidth)) {
		this -> m_cellWidth = value;
	}
	else if (StringMatch(sk, VAR_NACTFR)) {
		this -> m_nactfr = value;
	}
	else if (StringMatch(sk, VAR_CMN)) {
		this -> m_cmn = value;
	}
	else if (StringMatch(sk, VAR_WSHD_DNIT)) {
		this -> m_wshd_dnit = value;
	} 
	else if (StringMatch(sk, VAR_WSHD_HMN)) {
		this -> m_wshd_hmn = value;
	}
	else if (StringMatch(sk, VAR_WSHD_HMP)) {
		this -> m_wshd_hmp = value;
	} 
	else if (StringMatch(sk, VAR_WSHD_RMN)) {
		this -> m_wshd_rmn = value;
	}
	else if (StringMatch(sk, VAR_WSHD_RMP)) {
		this -> m_wshd_rmp = value;
	} 
	else if (StringMatch(sk, VAR_WSHD_RWN)) {
		this -> m_wshd_rwn = value;
	}
	else if (StringMatch(sk, VAR_WSHD_NITN)) {
		this -> m_wshd_nitn = value;
	}
	else if (StringMatch(sk, VAR_WSHD_VOLN)) {
		this -> m_wshd_voln = value;
	}
	else if (StringMatch(sk, VAR_WSHD_PAL)) {
		this -> m_wshd_pal = value;
	}
	else if (StringMatch(sk, VAR_WSHD_PAS)) {
		this -> m_wshd_pas = value;
	}
	else {
		throw ModelException("CLIMATE","SetValue","Parameter " + sk + " does not exist in CLIMATE method. Please contact the module developer.");
	}
}
void NandPim::Set1DData(const char* key,int n, float *data)
{
	if(!this->CheckInputSize(key,n)) return;

	string sk(key);
	if (StringMatch(sk, VAR_CDN)) {
		this -> m_cdn = data;
	} 
	else if (StringMatch(sk, VAR_LCC)) {
		this -> m_idplt = data;
	}
	else if (StringMatch(sk, VAR_PL_RSDCO)) {
		this -> m_rsdco_pl = data;
	}
	else if (StringMatch(sk, VAR_PSP)) {
		this -> m_psp = data;
	}
	else {
		throw ModelException("NMINRL","SetValue","Parameter " + sk + " does not exist in CLIMATE module. Please contact the module developer.");
	}
}
void NandPim::Set2DData(const char* key, int nRows, int nCols, float** data)
{
	if(!this->CheckInputSize(key,nCols)) return;

	string sk(key);
	if (StringMatch(sk, VAR_SOL_CBN)) {
		this -> m_sol_cbn = data;
	}
	else if (StringMatch(sk, VAR_SOL_WST)) {
		this -> m_sol_st = data;
	}
	else if (StringMatch(sk, VAR_SOL_WFC)) {
		this -> m_sol_fc = data;
	}
	else if (StringMatch(sk, VAR_SOL_TMP)) {
		this -> m_sol_tmp = data;
	}
	else if (StringMatch(sk, VAR_SOL_WH)) {
		this -> m_sol_ul = data;
	}
	else if (StringMatch(sk, VAR_SOL_AORGN)) {
		this -> m_sol_aorgn = data;
	}
	else if (StringMatch(sk, VAR_SOL_FON)) {
		this -> m_sol_fon = data;
	}
	else if (StringMatch(sk, VAR_SOL_FOP)) {
		this -> m_sol_fop = data;
	}
	else if (StringMatch(sk, VAR_SOL_NO3)) {
		this -> m_sol_no3 = data;
	}
	else if (StringMatch(sk, VAR_SOL_ORGN)) {
		this -> m_sol_orgn = data;
	}
	else if (StringMatch(sk, VAR_SOL_ORGP)) {
		this -> m_sol_orgp = data;
	}
	else if (StringMatch(sk, VAR_SOL_RSD)) {
		this -> m_sol_rsd = data;
	}
	else if (StringMatch(sk, VAR_SOL_SOLP)) {
		this -> m_sol_solp = data;
	}
	else if (StringMatch(sk, VAR_SOL_NH3)) {
		this -> m_sol_nh3 = data;
	}
	else if (StringMatch(sk, VAR_SOL_WPMM)) {
		this -> m_sol_wpmm = data;
	}
	else if (StringMatch(sk, VAR_ROOTDEPTH)) {
		this -> m_sol_z = data;
	}
	else if (StringMatch(sk, VAR_SOL_ACTP)) {
		this -> m_sol_actp = data;
	}
	else if (StringMatch(sk, VAR_SOL_STAP)) {
		this -> m_sol_stap = data;
	}
	else {
		throw ModelException("NMINRL","SetValue","Parameter " + sk + " does not exist in CLIMATE module. Please contact the module developer.");
	}
}
int NandPim::Execute() {
	if(!this -> CheckInputData()) { 
		return false;
	}
	//Calculate daily nitrogen and phosphorus mineralization and immobilization
	CalculateMinerandImmobi();
	//Calculate daily mineralization (NH3 to NO3) and volatilization of NH3
	CalculateMinerandVolati();
	//Calculate P flux between the labile, active mineral and stable mineral P pools
	CalculatePflux();
}
void NandPim::CalculateMinerandImmobi() {
	//soil layer (k)
	for(int k = 0; k < m_nLayers; k++) {
		//soil layer used to compute soil water and soil temperature factors
		int kk = 0;
		if(k == 0) {
			kk = 1;
		} else {
			kk = k;
		}
		for(int i = 0; i < m_nCells; i++) {
			float sut = 0;
			// mineralization can occur only if temp above 0 deg
			if (m_sol_tmp[kk][i] > 0) {
				// compute soil water factor (sut)
				sut = 0;
				if (m_sol_st[kk][i] < 0) {
					m_sol_st[kk][i] = 0.0000001f;
				}
				sut = 0.1 + 0.9 * sqrt(m_sol_st[kk][i] / m_sol_fc[kk][i]);
				sut = max(0.05, sut);

				//compute soil temperature factor
				//variable to hold intermediate calculation result (xx)
				float xx = 0;
				//soil temperature factor (cdg)
				float cdg = 0;
				xx = m_sol_tmp[kk][i];
				cdg = 0.9 * xx / (xx + exp(9.93 - 0.312 * xx)) + 0.1;
				cdg = max(0.1, cdg);

				// compute combined factor
				xx = 0;
				//combined temperature/soil water factor (csf)
				float csf = 0;
				xx = cdg * sut;
				if(xx < 0) {
					xx = 0;
				}
				if(xx > 1000000) {
					xx = 1000000;
				}
				csf = sqrt(xx);

				// compute flow from active to stable pools
				//amount of nitrogen moving from active organic to stable organic pool in layer (rwn)
				float rwn = 0;
				rwn = 0.1e-4 * (m_sol_aorgn[k][i] * (1 / m_nactfr - 1.) - m_sol_orgn[k][i]);
				if (rwn > 0.) {
					rwn = min(rwn, m_sol_aorgn[k][i]);
				} else {
					rwn = -(min(abs(rwn), m_sol_orgn[k][i]));
				}
				m_sol_orgn[k][i] = max(1.e-6, m_sol_orgn[k][i] + rwn);
				m_sol_aorgn[k][i] = max(1.e-6, m_sol_aorgn[k][i] - rwn);

				// compute humus mineralization on active organic n
				//amount of nitrogen moving from active organic nitrogen pool to nitrate pool in layer (hmn)
				float hmn = 0;
				hmn = m_cmn * csf * m_sol_aorgn[k][i];
				hmn = min(hmn, m_sol_aorgn[k][i]);
				// compute humus mineralization on active organic p
				xx = 0;
				//amount of phosphorus moving from the organic pool to the labile pool in layer (hmp)
				float hmp = 0;
				xx = m_sol_orgn[k][i] + m_sol_aorgn[k][i];
				if (xx > 1e-6) {
					hmp = 1.4 * hmn * m_sol_orgp[k][i] / xx;
				} else {
					hmp = 0;
				}
				hmp = min(hmp, m_sol_orgp[k][i]);

				// move mineralized nutrients between pools;
				m_sol_aorgn[k][i] = max(1e-6, m_sol_aorgn[k][i] - hmn);
				m_sol_no3[k][i] = m_sol_no3[k][i] + hmn;
				m_sol_orgp[k][i] = m_sol_orgp[k][i] - hmp;
				m_sol_solp[k][i] = m_sol_solp[k][i] + hmp;

				// compute residue decomp and mineralization of 
				// fresh organic n and p (upper two layers only)
				//amount of nitrogen moving from fresh organic to nitrate(80%) and active organic(20%) pools in layer (rmn1)
				float rmn1 = 0;
				//amount of phosphorus moving from fresh organic to labile(80%) and organic(20%) pools in layer (rmp)
				float rmp = 0;
				if (k <= 2) {
					//the C:N ratio (cnr)
					float cnr = 0;
					//the C:P ratio (cnr)
					float cpr = 0;
					//the nutrient cycling water factor for layer (ca)
					float ca = 0;
					float cnrf = 0;
					float cprf = 0;
					if (m_sol_fon[k][i] + m_sol_no3[k][i] > 1e-4) {
						//Calculate cnr, equation 3:1.2.5 in SWAT Theory 2009, p189
						cnr = 0.58 * m_sol_rsd[k][i] / (m_sol_fon[k][i] + m_sol_no3[k][i]);
						if (cnr > 500) {
							cnr = 500;
						}
						cnrf = exp(-0.693 * (cnr - 25) / 25);
					} else {
						cnrf = 1;
					}
					if (m_sol_fop[k][i] + m_sol_solp[k][i] > 1e-4) {
						cpr = 0.58 * m_sol_rsd[k][i] / (m_sol_fop[k][i] + m_sol_solp[k][i]);
						if (cpr > 5000) {
							cpr = 5000;
						}
						cprf = 0.;
						cprf = exp(-0.693 * (cpr - 200) / 200);
					} else {
						cprf = 1;
					}
					//decay rate constant (decr)
					float decr = 0;

					float rdc = 0;
					//Calculate ca, equation 3:1.2.8 in SWAT Theory 2009, p190
					ca = min(min(cnrf, cprf), 1);
					if (m_idplt[i] > 0) {
						decr = m_rsdco_pl[(int)m_idplt[i]] * ca * csf;
					} else {
						decr = 0.05;
					}
					decr = min(decr, 1.0f);
					m_sol_rsd[k][i] = max(1.e-6, m_sol_rsd[k][i]);
					rdc = decr * m_sol_rsd[k][i];
					m_sol_rsd[k][i] = m_sol_rsd[k][i] - rdc;
					if (m_sol_rsd[k][i] < 0) {
						m_sol_rsd[k][i] = 0;
					}
					rmn1 = decr * m_sol_fon[k][i];
					m_sol_fop[k][i] = max(1e-6, m_sol_fop[k][i]);
					rmp = decr * m_sol_fop[k][i];

					m_sol_fop[k][i] = m_sol_fop[k][i] - rmp;
					m_sol_fon[k][i] = max(1e-6, m_sol_fon[k][i]) - rmn1;;
					//Calculate no3, aorgn, solp, orgp, equation 3:1.2.9 in SWAT Theory 2009, p190
					m_sol_no3[k][i] = m_sol_no3[k][i] + 0.8 * rmn1;
					m_sol_aorgn[k][i] = m_sol_aorgn[k][i] + 0.2 * rmn1;
					m_sol_solp[k][i] = m_sol_solp[k][i] + 0.8 * rmp;
					m_sol_orgp[k][i] = m_sol_orgp[k][i] + 0.2 * rmp;
				}
				//  compute denitrification
				//amount of nitrogen lost from nitrate pool in layer due to denitrification
				float wdn = 0; 
				//denitrification threshold: fraction of field capacity (sdnco)
				float* sdnco(NULL);////////////////////////////////////////////////////////////////////////parameter
				//Calculate wdn, equation 3:1.4.1 and 3:1.4.2 in SWAT Theory 2009, p194
				if (sut >= sdnco[i]) {
					wdn = m_sol_no3[k][i] * (1. - exp(-m_cdn[i] * cdg * m_sol_cbn[k][i]));
				} else {
					wdn = 0;
				}
				m_sol_no3[k][i] = m_sol_no3[k][i] - wdn;

				// Summary calculations
				m_wshd_hmn = m_wshd_hmn + hmn * (1 / m_nCells);
				m_wshd_rwn = m_wshd_rwn + rwn * (1 / m_nCells);
				m_wshd_hmp = m_wshd_hmp + hmp * (1 / m_nCells);
				m_wshd_rmn = m_wshd_rmn + rmn1 * (1 / m_nCells);
				m_wshd_rmp = m_wshd_rmp + rmp * (1 / m_nCells);
				m_wshd_dnit = m_wshd_dnit + wdn * (1 / m_nCells);
				m_hmntl = m_hmntl + hmn;
				m_rwntl = m_rwntl + rwn;
				m_hmptl = m_hmptl + hmp;
				m_rmn2tl = m_rmn2tl + rmn1;
				m_rmptl = m_rmptl + rmp;
				m_wdntl = m_wdntl + wdn;
			}
		}
	}
}
void NandPim::CalculateMinerandVolati() {
	//soil layer (k)
	int kk = 0;
	for(int k = 0; k < m_nLayers; k++) {
		for(int i = 0; i < m_nCells; i++) {
			//nitrification/volatilization temperature factor (nvtf)
			float nvtf = 0;
			//Calculate nvtf, equation 3:1.3.1 in SWAT Theory 2009, p192
			nvtf = 0.41 * (m_sol_tmp[k][i] - 5) / 10;
			if (m_sol_nh3[k][i] > 0 && nvtf >= 0.001) {
				float sw25 = 0;
				float swwp = 0;
				//nitrification soil water factor (swf)
				float swf = 0;
				//Calculate nvtf, equation 3:1.3.2 and 3:1.3.3 in SWAT Theory 2009, p192
				sw25 = m_sol_wpmm[k][i] + 0.25 * m_sol_fc[k][i];
				swwp = m_sol_wpmm[k][i] + m_sol_st[k][i];
				if (swwp < sw25) {
					swf = (swwp - m_sol_wpmm[k][i]) / (sw25 - m_sol_wpmm[k][i]);
				} else {
					swf = 1;
				}

				if (k == 0) {
					kk = 0;
				} else {
					kk = m_sol_z[k-1][i];
				}
				//depth from the soil surface to the middle of the layer (dmidl/mm)
				float dmidl = 0;
				//volatilization depth factor (dpf)
				float dpf = 0;
				//nitrification regulator (akn)
				float akn = 0;
				//volatilization regulator (akn)
				float akv = 0;
				//amount of ammonium converted via nitrification and volatilization in layer (rnv)
				float rnv = 0;
				//amount of nitrogen moving from the NH3 to the NO3 pool (nitrification) in the layer (rnit)
				float rnit = 0;
				//amount of nitrogen lost from the NH3 pool due to volatilization (rvol)
				float rvol = 0;
				//volatilization CEC factor (cecf)
				float cecf = 0.15;
				//Calculate nvtf, equation 3:1.3.2 and 3:1.3.3 in SWAT Theory 2009, p192
				dmidl = (m_sol_z[k][i] + kk) / 2;
				dpf = 1 - dmidl / (dmidl + exp(4.706 - 0.0305 * dmidl));
				//Calculate rnv, equation 3:1.3.6, 3:1.3.7 and 3:1.3.8 in SWAT Theory 2009, p193
				akn = nvtf * swf;
				akv = nvtf * dpf * cecf;
				rnv = m_sol_nh3[k][i] * (1 - exp(-akn - akv));
				//Calculate rnit, equation 3:1.3.9 in SWAT Theory 2009, p193
				rnit = 1 - exp(-akn);
				//Calculate rvol, equation 3:1.3.10 in SWAT Theory 2009, p193
				rvol = 1 - exp(-akv);

				//calculate nitrification (NH3 => NO3)
				if ((rvol + rnit) > 1e-6) {
					//Calculate the amount of nitrogen removed from the ammonium pool by nitrification,
					//equation 3:1.3.11 in SWAT Theory 2009, p193
					rvol = rnv * rvol / (rvol + rnit);
					//Calculate the amount of nitrogen removed from the ammonium pool by volatilization, 
					//equation 3:1.3.12 in SWAT Theory 2009, p194
					rnit = rnv - rvol;
					if (rnit < 0) {
						rnit = 0;
					}
					m_sol_nh3[k][i] = max(1e-6, m_sol_nh3[k][i] - rnit);
				}
				if (m_sol_nh3[k][i] < 0) {
					rnit = rnit + m_sol_nh3[k][i];
					m_sol_nh3[k][i] = 0;
				}
				m_sol_no3[k][i] = m_sol_no3[k][i] + rnit;
				//calculate ammonia volatilization
				m_sol_nh3[k][i] = max(1e-6, m_sol_nh3[k][i] - rvol);
				if (m_sol_nh3[k][i] < 0) {
					rvol = rvol + m_sol_nh3[k][i];
					m_sol_nh3[k][i] = 0;
				}
				//summary calculations
				m_wshd_voln = m_wshd_voln + rvol * (1 / m_nCells);
				m_wshd_nitn = m_wshd_nitn + rnit * (1 / m_nCells);
			}
		}
	}
}
void NandPim::CalculatePflux() {
	//slow equilibration rate constant (bk)
	float bk = 0.0006;
	for(int i = 0; i < m_nCells; i++) {
		float rto = m_psp[i] / (1 - m_psp[i]);
		for(int k = 0; k < m_nLayers; k++) {
			float rmn1 = 0;
			rmn1 = (m_sol_solp[i][1] - m_sol_actp[i][1] * rto);
				if (rmn1 > 0.) rmn1 = rmn1 * 0.1;
				if (rmn1 < 0.) {
					rmn1 = rmn1 * 0.6;
				}
				rmn1 = min(rmn1, m_sol_solp[i][1]);
				//amount of phosphorus moving from the active mineral to the stable mineral pool in the soil layer (roc)
				float roc = 0;
				//Calculate roc, equation 3:2.3.4 and 3:2.3.5 in SWAT Theory 2009, p215
				roc = bk * (4. * m_sol_actp[i][1] - m_sol_stap[i][1]);
				if (roc < 0) {
					roc = roc * 0.1;
				}
				roc = min(roc, m_sol_actp[i][1]);
				m_sol_stap[i][1] = m_sol_stap[i][1] + roc;
				if (m_sol_stap[i][1] < 0) {
					m_sol_stap[i][1] = 0;
				}
				m_sol_actp[i][1] = m_sol_actp[i][1] - roc + rmn1;
				if (m_sol_actp[i][1] < 0) {
					m_sol_actp[i][1] = 0;
				}
				m_sol_solp[i][1] = m_sol_solp[i][1] - rmn1;
				if (m_sol_solp[i][1] < 0) {
					m_sol_solp[i][1] = 0;
				}
				m_wshd_pas = m_wshd_pas + roc * (1 / m_nCells);
				m_wshd_pal = m_wshd_pal + rmn1 *(1 / m_nCells);
				m_roctl = m_roctl + roc;
				m_rmp1tl = m_rmp1tl + rmn1;
		}
	}
}
void NandPim::GetValue(const char* key, float* value) {
	string sk(key);
	if (StringMatch(sk, VAR_HMNTL)) {
		*value = this -> m_hmntl;
	}
	else if (StringMatch(sk, VAR_HMPTL)) {
		*value = this -> m_hmptl;
	}
	else if (StringMatch(sk, VAR_RMN2TL)) {
		*value = this -> m_rmn2tl;
	}
	else if (StringMatch(sk, VAR_RMPTL)) {
		*value = this -> m_rmptl;
	}
	else if (StringMatch(sk, VAR_RWNTL)) {
		*value = this -> m_rwntl;
	}
	else if (StringMatch(sk, VAR_WDNTL)) {
		*value = this -> m_wdntl;
	}
	else if (StringMatch(sk, VAR_RMP1TL)) {
		*value = this -> m_rmp1tl;
	}
	else if (StringMatch(sk, VAR_ROCTL)) {
		*value = this -> m_roctl;
	}
	else if (StringMatch(sk, VAR_WSHD_DNIT)) {
		*value = this -> m_wshd_dnit;
	}
	else if (StringMatch(sk, VAR_WSHD_HMN)) {
		*value = this -> m_wshd_hmn;
	}
	else if (StringMatch(sk, VAR_WSHD_HMP)) {
		*value = this -> m_wshd_hmp;
	}
	else if (StringMatch(sk, VAR_WSHD_RMN)) {
		*value = this -> m_wshd_rmn;
	}
	else if (StringMatch(sk, VAR_WSHD_RMP)) {
		*value = this -> m_wshd_rmp;
	}
	else if (StringMatch(sk, VAR_WSHD_RWN)) {
		*value = this -> m_wshd_rwn;
	}
	else if (StringMatch(sk, VAR_WSHD_NITN)) {
		*value = this -> m_wshd_nitn;
	}
	else if (StringMatch(sk, VAR_WSHD_VOLN)) {
		*value = this -> m_wshd_voln;
	}
	else if (StringMatch(sk, VAR_WSHD_PAL)) {
		*value = this -> m_wshd_pal;
	}
	else if (StringMatch(sk, VAR_WSHD_PAS)) {
		*value = this -> m_wshd_pas;
	}
	else {
		throw ModelException("NMINRL", "GetValue","Parameter " + sk + " does not exist. Please contact the module developer.");
	}
}	
void NandPim::Get2DData(const char* key, int *nRows, int *nCols, float*** data) {
	string sk(key);
	*nRows = m_nLayers;
	*nCols = m_nCells;
	if (StringMatch(sk, VAR_SOL_AORGN)) {
		*data = this -> m_sol_aorgn; 
	}
	else if (StringMatch(sk, VAR_SOL_FON)) {
		*data = this -> m_sol_fon;
	}
	else if (StringMatch(sk, VAR_SOL_FOP)) {
		*data = this -> m_sol_fop;
	}
	else if (StringMatch(sk, VAR_SOL_NO3)) {
		*data = this -> m_sol_no3;
	}
	else if (StringMatch(sk, VAR_SOL_ORGN)) {
		*data = this -> m_sol_orgn;
	}
	else if (StringMatch(sk, VAR_SOL_ORGP)) {
		*data = this -> m_sol_orgp;
	}
	else if (StringMatch(sk, VAR_SOL_RSD)) {
		*data = this -> m_sol_rsd;
	}
	else if (StringMatch(sk, VAR_SOL_SOLP)) {
		*data = this -> m_sol_solp;
	}
	else if (StringMatch(sk, VAR_SOL_NH3)) {
		*data = this -> m_sol_nh3;
	}
	else if (StringMatch(sk, VAR_SOL_ACTP)) {
		*data = this -> m_sol_actp;
	}
	else if (StringMatch(sk, VAR_SOL_STAP)) {
		*data = this -> m_sol_stap;
	}
	else
		throw ModelException("Denitrification", "Get2DData", "Output " + sk 
		+ " does not exist in the Denitrification module. Please contact the module developer.");
}
// int main() {
	//	system("pause");
//}