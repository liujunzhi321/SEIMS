/*//
 * \file NutrientCHRoute.cpp
 * \ingroup NutCHRout
 * \author Huiran Gao
 * \date Jun 2016
 */

#include <iostream>
#include "NutrientCHRoute.h"
#include "MetadataInfo.h"
#include <cmath>
#include <fstream>
#include "ModelException.h"
#include "util.h"
#include <omp.h>
using namespace std;

NutrientCHRoute::NutrientCHRoute(void):
//input
m_nCells(-1), m_cellWidth(-1), m_gwno3(NULL), m_gwminp(NULL), m_gw_q(NULL),
	//output
	m_minpgw(NULL), m_no3gw(NULL)
{

}

NutrientCHRoute::~NutrientCHRoute(void) {
	///TODO
}
bool NutrientCHRoute::CheckInputSize(const char* key, int n) {
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
bool NutrientCHRoute::CheckInputData() {
	if(this->m_nCells == -1) {
		throw ModelException("NutRemv","CheckInputData","You have not set the date time.");
		return false;
	}
	if(m_nCells <= 0) {
		throw ModelException("NutRemv","CheckInputData","The dimension of the input data can not be less than zero.");
		return false;
	}
	if(this->m_cellWidth == NULL) {
		throw ModelException("NutRemv","CheckInputData","The relative humidity can not be NULL.");
		return false;
	}
	///...
	return true;
}
void NutrientCHRoute::SetValue(const char* key, float value)
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
		throw ModelException("NutRemv","SetValue","Parameter " + sk + " does not exist in CLIMATE method. Please contact the module developer.");
	}
}
void NutrientCHRoute::Set1DData(const char* key,int n, float *data)
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
		throw ModelException("NutRemv","SetValue","Parameter " + sk + " does not exist in CLIMATE module. Please contact the module developer.");
	}
}

int NutrientCHRoute::Execute() {
	if(!this -> CheckInputData()) { 
		return false;
	}
	map<int, vector<int> >::iterator it;
	for (it = m_reachLayers.begin(); it != m_reachLayers.end(); it++)
	{
		// There are not any flow relationship within each routing layer.
		// So parallelization can be done here.
		int nReaches = it->second.size();
		// the size of m_reachLayers (map) is equal to the maximum stream order
		/// #pragma omp parallel for
		for (int i = 0; i < nReaches; ++i)
		{
			// index in the array
			int reachIndex = it->second[i]; 
			NutrientinChannel(reachIndex);
		}
	}
	//return ??
	return 0;
}
void NutrientCHRoute::NutrientinChannel(int i){

	float thbc1 = 1.083;    ///temperature adjustment factor for local biological oxidation of NH3 to NO2
	float thbc2 = 1.047;    ///temperature adjustment factor for local biological oxidation of NO2 to NO3
	float thbc3 = 1.04;    ///temperature adjustment factor for local hydrolysis of organic N to ammonia N
	float thbc4 = 1.047;    ///temperature adjustment factor for local decay of organic P to dissolved P

	float thgra = 1.047;    ///temperature adjustment factor for local algal growth rate
	float thrho = 1.047;    ///temperature adjustment factor for local algal respiration rate

	float thm_rk1 = 1.047;    ///temperature adjustment factor for local CBOD deoxygenation
	float thm_rk2 = 1.024;    ///temperature adjustment factor for local oxygen reaeration rate
	float thm_rk3 = 1.024;    ///temperature adjustment factor for loss of CBOD due to settling
	float thm_rk4 = 1.060;    ///temperature adjustment factor for local sediment oxygen demand

	float thrs1 = 1.024;    ///temperature adjustment factor for local algal settling rate
	float thrs2 = 1.074;    ///temperature adjustment factor for local benthos source rate for dissolved phosphorus
	float thrs3 = 1.074;    ///temperature adjustment factor for local benthos source rate for ammonia nitrogen
	float thrs4 = 1.024;    ///temperature adjustment factor for local organic N settling rate
	float thrs5 = 1.024;    ///temperature adjustment factor for local organic P settling rate

	// initialize inflow concentrations
	float chlin = 0.;
	float algin = 0.;
	float orgnin = 0.;
	float ammoin = 0.;
	float nitritin = 0.;
	float nitratin = 0.;
	float orgpin = 0.;
	float dispin = 0.;
	float cbodin = 0.;
	float disoxin = 0.;
	float cinn = 0.;
	float heatin = 0.;

	float dalgae = 0.;
	float dchla = 0.;
	float dbod = 0.;
	float ddisox = 0.;
	float dorgn = 0.;
	float dnh4  = 0.;
	float dno2 = 0.;
	float dno3  = 0.;
	float dorgp = 0.;
	float dsolp  = 0.;
	// initialize water flowing into reach
	float wtrIn = 0.;
	float qiSub = 0.f;
	if(m_qiSub != NULL)
		qiSub = m_qiSub[i];
	float qgSub = 0.f;
	if(m_qgSub != NULL)
		qgSub = m_qgSub[i];
	// 1. water from this subbasin
	wtrIn = m_qsSub[i] + qiSub + qgSub;
	// 2. water from upstream reaches
	float qsUp = 0.f;
	float qiUp = 0.f;
	float qgUp = 0.f;
	for (size_t j = 0; j < m_reachUpStream[i].size(); ++j) {
		int upReachId = m_reachUpStream[i][j];
		qsUp += m_qsCh[upReachId];
		qiUp += m_qiCh[upReachId];
		qgUp += m_qgCh[upReachId];
	}
	wtrIn += qsUp + qiUp + qgUp;
	// m_qUpReach is zero for not-parallel program and qsUp, qiUp and qgUp are zero for parallel computing
	wtrIn += m_qUpReach;
	// 3. water from bank storage
	float bankOut = 0.;
	bankOut = m_bankStorage[i] * (1 - exp(- m_aBank));
	m_bankStorage[i] -= bankOut;
	wtrIn += bankOut/m_dt;

	//wtrIn = varoute[][inum2)] * (1. - m_rnum1);
	// Calculate flow out of reach
	float wtrOut = 0.;
	wtrOut = m_qsCh[i] + m_qiCh[i] + m_qgCh[i];

	// Concentrations
	if ((wtrOut / 86400.) > 0.01 && wtrIn > 0.001) {
		// initialize concentration of nutrient in reach
		// inflow + storage water (wtrTotal m3)
		float wtrTotal = 0.;
		// initial algal biomass concentration in reach (algcon mg/L)
		float algcon = 0.;
		// initial organic N concentration in reach (orgncon mg/L)
		float orgncon = 0.;
		// initial ammonia concentration in reach (nh3con mg/L)
		float nh3con = 0.;
		// initial nitrite concentration in reach (no2con mg/L)
		float no2con = 0.;
		// initial nitrate concentration in reach (no3con mg/L)
		float no3con = 0.;
		// initial organic P concentration in reach (orgpcon  mg/L)
		float orgpcon = 0.;
		// initial soluble P concentration in reach (solpcon mg/L)
		float solpcon = 0.;
		// initial carbonaceous biological oxygen demand (cbodcon mg/L)
		float cbodcon = 0.;
		// initial dissolved oxygen concentration in reach (o2con mg/L)
		float o2con = 0.;
		wtrTotal = wtrIn + m_chStorage[i];

		algcon = m_algae[i];
		orgncon = m_organicn[i];
		nh3con = m_ammonian[i];
		no2con = m_nitriten[i];
		no3con = m_nitraten[i];
		orgpcon =  m_organicp[i];
		solpcon = m_disolvp[i];
		cbodcon = m_rch_cbod[i];
		o2con  = m_rch_dox[i];
		// temperature of water in reach (wtmp deg C)
		float wtmp = 0.;
		wtmp = m_wattemp[i];
		// calculate temperature in stream
		if (wtmp <= 0.) {
			wtmp = 0.1;
		}

		// calculate effective concentration of available nitrogen (cinn)
		float cinn = 0.;
		cinn = nh3con + no3con;

		// calculate saturation concentration for dissolved oxygen
		float ww = 0.;    // variable to hold intermediate calculation result
		float xx = 0.;     // variable to hold intermediate calculation result
		float yy = 0.;     // variable to hold intermediate calculation result
		float zz = 0.;     // variable to hold intermediate calculation result
		ww = -139.34410 + (1.575701e05 / (wtmp + 273.15));
		xx = 6.642308e+07 / pow((wtmp + 273.15), 2);
		yy = 1.243800e+10 / pow((wtmp + 273.15), 3);
		zz = 8.621949e+11 / pow((wtmp + 273.15), 4);
		m_soxy = exp(ww - xx + yy - zz);
		if (m_soxy < 1.e-6) {
			m_soxy = 0. ;
		}

		// O2 impact calculations
		// calculate nitrification rate correction factor for low (cordo)
		float cordo = 0.;
		float o2con2 = 0.;
		o2con2 = o2con;
		if (o2con2 <= 0.1) {
			o2con2=0.1;
		}
		if (o2con2 > 30.) {
			o2con2=30.;
		}
		cordo = 1.0 - exp(-0.6 * o2con2);
		if (o2con<=0.001) {
			o2con=0.001;
		}
		if (o2con>30.) {
			o2con=30.;
		}
		cordo = 1.0 - exp(-0.6 * o2con);

		// modify ammonia and nitrite oxidation rates to account for low oxygen
		// rate constant for biological oxidation of NH3 to NO2 modified to reflect impact of low oxygen concentration (bc1mod)
		float bc1mod = 0.;
		// rate constant for biological oxidation of NO2 to NO3 modified to reflect impact of low oxygen concentration (bc1mod)
		float bc2mod = 0.;
		bc1mod = m_bc1[i] * cordo;
		bc2mod = m_bc2[i] * cordo;

		//	tday is the calculation time step = 1 day
		float tday = 1.0;

		// algal growth
		// calculate light extinction coefficient (m_lambda)
		if (m_ai0 * algcon > 1.e-6) {
			m_lambda = m_lambda0 + (m_lambda1 * m_ai0 * algcon) + m_lambda2 * pow((m_ai0 * algcon), (2 / 3));
		} else {
			m_lambda = m_lambda0;
		}

		// calculate algal growth limitation factors for nitrogen and phosphorus
		float fnn = 0.;
		float fpp = 0.;
		fnn = cinn / (cinn + m_k_n);
		fpp = solpcon / (solpcon + m_k_p);

		// calculate daylight average, photo synthetically active (algi)
		float algi = 0.;
		float dchla = 0.;
		float dbod = 0.;
		float ddisox = 0.;
		float dorgn = 0.;
		float dnh4  = 0.;
		float dno2 = 0.;
		float dno3  = 0.;
		float dorgp = 0.;
		float dsolp  = 0.;
		if (m_dayl[i] > 0.) {
			algi = m_sra[i] * tfact / m_dayl[i];
		} else {
			algi = 0.00001;
		}

		// calculate growth attenuation factor for light, based on daylight average light intensity
		float fl_1 = 0.;
		float fll = 0.;
		fl_1 = (1. / (m_lambda * m_chWTdepth[i])) * log((m_k_l + algi) / (m_k_l + algi * exp(-m_lambda * m_chWTdepth[i])));
		fll = 0.92 * (m_dayl[i] / 24.) * fl_1;

		// calculate local algal growth rate
		float gra = 0.;
		float dchla = 0.;
		float dbod = 0.;
		float ddisox = 0.;
		float dorgn = 0.;
		float dnh4  = 0.;
		float dno2 = 0.;
		float dno3  = 0.;
		float dorgp = 0.;
		float dsolp  = 0.;
		switch (igropt) {
			case 1:
				// multiplicative
				gra = m_mumax * fll * fnn * fpp;
			case 2:
				// limiting nutrient
				gra = m_mumax * fll * min(fnn, fpp);
			case 3:
				// harmonic mean
				if (fnn > 1.e-6 && fpp > 1.e-6) {
					gra = m_mumax * fll * 2. / ((1. / fnn) + (1. / fpp));
				} else {
					gra = 0.;
				}
		}

		// calculate algal biomass concentration at end of day (phytoplanktonic algae)
		dalgae = 0.;
		float setl = min(1.f, corTempc(m_rs1[i], thrs1, wtmp) / m_chWTdepth[i]);
		dalgae = algcon + (corTempc(gra, thgra, wtmp) * algcon - corTempc(m_rhoq, thrho, wtmp) * algcon - setl * algcon) * tday;
		if (dalgae < 0.00001)  {
			m_algae[i] = 0.00001;
		}
		// calculate chlorophyll-a concentration at end of day
		dchla = 0.;
		dchla = dalgae * m_ai0 / 1000.;
		
		// oxygen calculations
		// calculate carbonaceous biological oxygen demand at end of day (dbod)
		float yyy = 0.;     // variable to hold intermediate calculation result
		float zzz = 0.;     // variable to hold intermediate calculation result
		yyy = corTempc(m_rk4[i], thm_rk1, wtmp) * cbodcon;
		zzz = corTempc(m_rk3[i], thm_rk3, wtmp) * cbodcon;
		dbod = 0.;
		dbod = cbodcon - (yyy + zzz) * tday;
		if (dbod < 0.00001) {
			dbod = 0.00001;
		}

		// calculate dissolved oxygen concentration if reach at end of day (ddisox)
		float uu = 0.;     // variable to hold intermediate calculation result
		float vv = 0.;      // variable to hold intermediate calculation result
		float ww = 0.;    // variable to hold intermediate calculation result
		xx = 0.;     // variable to hold intermediate calculation result
		yy = 0.;     // variable to hold intermediate calculation result
		zz = 0.;     // variable to hold intermediate calculation result
		float hh = corTempc(m_rk2[i], thm_rk2, wtmp) ;
		float uu = corTempc(m_rk2[i], thm_rk2, wtmp) * (m_soxy - o2con);
		if (algcon > 0.001) {
			vv = (m_ai3 * corTempc(gra, thgra, wtmp) - m_ai4 * corTempc(m_rhoq, thrho, wtmp)) * algcon;
		} else {
			algcon = 0.001;
		}
		ww = corTempc(m_rk1[i],thm_rk1,wtmp) * cbodcon;
		if(m_chWTdepth[i] > 0.001) {
			xx = corTempc(m_rk4[i],thm_rk4,wtmp) / (m_chWTdepth[i] * 1000.);
		}
		if (nh3con > 0.001) {
			yy = m_ai5 * corTempc(bc1mod,thbc1,wtmp) * nh3con;
		} else {
			nh3con = 0.001;
		}
		if (no2con > 0.001) {
			zz = m_ai6 * corTempc(bc2mod, thbc2, wtmp) * no2con;
		} else {
				no2con=0.001;
		}
		ddisox = 0.;
		ddisox = o2con + (uu + vv - ww - xx - yy - zz) * tday;
		//o2proc = o2con - ddisox;   // not found variable "o2proc"
		if (ddisox < 0.00001) {
			ddisox = 0.00001 ;
		}
		
		// nitrogen calculations
		// calculate organic N concentration at end of day (dorgn)
		xx = 0.;
		yy = 0.;
		zz = 0.;
		xx = m_ai1 * corTempc(m_rhoq, thrho, wtmp) * algcon;
		yy = corTempc(m_bc3[i], thbc3, wtmp) * orgncon;
		zz = corTempc(m_rs4[i], thrs4, wtmp) * orgncon;
		dorgn = 0.;
		dorgn = orgncon + (xx - yy - zz) * tday;
		if (dorgn < 0.00001) {
			dorgn = 0.00001;
		}
				
		// calculate fraction of algal nitrogen uptake from ammonia pool
		float f1 = 0.;
		f1 = m_p_n * nh3con / (m_p_n * nh3con + (1. - m_p_n) * no3con + 1.e-6);

		// calculate ammonia nitrogen concentration at end of day (dnh4)
		ww = 0.;
		xx = 0.;
		yy = 0.;
		zz = 0.;
		ww = corTempc(m_bc3[i], thbc3, wtmp) * orgncon;
		xx = corTempc(bc1mod, thbc1, wtmp) * nh3con;
		yy = corTempc(m_rs3[i], thrs3, wtmp) / (m_chWTdepth[i] * 1000.);
		zz = f1 * m_ai1 * algcon * corTempc(gra, thgra, wtmp);
		dnh4  = 0.;
		dnh4  = nh3con + (ww - xx + yy - zz) * tday;
		if (dnh4  < 1.e-6) {
			dnh4  = 0.;
		}

		// calculate concentration of nitrite at end of day (dno2)
		yy = 0.;
		zz = 0.;
		yy = corTempc(bc1mod,thbc1,wtmp) * nh3con;
		zz = corTempc(bc2mod,thbc2,wtmp) * no2con;
		dno2 = 0.;
		dno2 = no2con + (yy - zz) * tday;
		if (dno2 < 1.e-6) {
			dno2 = 0.;
		}

		// calculate nitrate concentration at end of day (dno3)
		yy = 0.;
		zz = 0.;
		yy = corTempc(bc2mod, thbc2, wtmp) * no2con;
		zz = (1. - f1) * m_ai1 * algcon * corTempc(gra, thgra, wtmp);
		dno3  = 0.;
		dno3  = no3con + (yy - zz) * tday;
		if (dno3 < 1.e-6) {
			dno3  = 0.;
		}
		
		// phosphorus calculations
		// calculate organic phosphorus concentration at end of day
		xx = 0.;
		yy = 0.;
		zz = 0.;
		xx = m_ai2 * corTempc(m_rhoq, thrho, wtmp) * algcon;
		yy = corTempc(m_bc4[i], thbc4, wtmp) * orgpcon;
		zz = corTempc(m_rs5[i], thrs5, wtmp) * orgpcon;
		dorgp = 0.;
		dorgp = orgpcon + (xx - yy - zz) * tday;
		if (dorgp < 1.e-6) {
			dorgp = 0.;
		}

		// calculate dissolved phosphorus concentration at end of day (dsolp)
		xx = 0.;
		yy = 0.;
		zz = 0.;
		xx = corTempc(m_bc4[i],thbc4,wtmp) * orgpcon;
		yy = corTempc(m_rs2[i],thrs2,wtmp) / (m_chWTdepth[i] * 1000.);
		zz = m_ai2 * corTempc(gra,thgra,wtmp) * algcon;
		dsolp  = 0.;
		dsolp  = solpcon + (xx + yy - zz) * tday;
		if (dsolp  < 1.e-6) dsolp  = 0.;

		wtrTotal = wtrIn + m_chStorage[i];

		if (wtrIn > 0.001) {
			chlin = 1000. * varoute[13][i] * (1. - m_rnum1) / wtrIn;
			algin = 1000. * chlin / m_ai0;
			orgnin = 1000. * varoute[4][i] * (1. - m_rnum1) / wtrIn;
			ammoin = 1000. * varoute[14][i] * (1. - m_rnum1) / wtrIn;
			nitritin = 1000. * varoute[15][i] * (1. - m_rnum1) / wtrIn;
			nitratin = 1000. * varoute[6][i] * (1. - m_rnum1) / wtrIn;
			orgpin = 1000. * varoute[5][i] * (1. - m_rnum1) / wtrIn;
			dispin = 1000. * varoute[7][i] * (1. - m_rnum1) / wtrIn;
			cbodin = 1000. * varoute[16][i] * (1. - m_rnum1) / wtrIn;
			disoxin= 1000. * varoute[17][i] * (1. - m_rnum1) / wtrIn;
			heatin = varoute[1][i] * (1. - m_rnum1);
		}

		m_wattemp[i] =(heatin * wtrIn + wtmp * m_chStorage[i]) / wtrTotal;
		m_algae[i] = (algin * wtrIn + dalgae * m_chStorage[i]) / wtrTotal;
		m_organicn[i] = (orgnin * wtrIn + dorgn * m_chStorage[i]) / wtrTotal;
		m_ammonian[i] = (ammoin * wtrIn +  dnh4 * m_chStorage[i]) / wtrTotal;
		m_nitriten[i] = (nitritin * wtrIn + dno2  * m_chStorage[i]) / wtrTotal;
		m_nitraten[i] = (nitratin * wtrIn + dno3  * m_chStorage[i]) / wtrTotal;
		m_organicp[i] = (orgpin * wtrIn +  dorgp * m_chStorage[i]) / wtrTotal;
		m_disolvp[i] = (dispin * wtrIn +  dsolp * m_chStorage[i]) / wtrTotal;
		m_rch_cbod[i] = (cbodin * wtrIn + dbod * m_chStorage[i]) / wtrTotal;
		m_rch_dox[i] = (disoxin * wtrIn +  ddisox * m_chStorage[i]) / wtrTotal;

		// calculate chlorophyll-a concentration at end of day
		m_chlora[i] = 0.;
		m_chlora[i] = m_algae[i] * m_ai0 / 1000.;
	} else {
		// all water quality variables set to zero when no flow
		algin = 0.0;
		chlin = 0.0;
		orgnin = 0.0;
		ammoin = 0.0;
		nitritin = 0.0;
		nitratin = 0.0;
		orgpin = 0.0;
		dispin = 0.0;
		cbodin = 0.0;
		disoxin = 0.0;
		m_algae[i] = 0.0;
		m_chlora[i] = 0.0;
		m_organicn[i] = 0.0;
		m_ammonian[i] = 0.0;
		m_nitriten[i] = 0.0;
		m_nitraten[i] = 0.0;
		m_organicp[i] = 0.0;
		m_disolvp[i] = 0.0;
		m_rch_cbod[i] = 0.0;
		m_rch_dox[i] = 0.0;
		dalgae = 0.0;
		dchla = 0.0;
		dorgn = 0.0;
		dnh4 = 0.0;
		dno2 = 0.0;
		dno3 = 0.0;
		dorgp= 0.0;
		dsolp = 0.0;
		dbod = 0.0;
		ddisox = 0.0;
		m_soxy = 0.0;
	}

}
float NutrientCHRoute::corTempc(float r20, float thk, float tmp) {
	float theta = 0.;
	theta = r20 * pow(thk, (tmp - 20.f));
	return theta;
}
void NutrientCHRoute::Get1DData(const char* key, int* n, float** data) {
	string sk(key);
	*n = m_nCells;
	if (StringMatch(sk, VAR_NO3GW)) {
		*data = this -> m_no3gw;
	}
	if (StringMatch(sk, VAR_MINPGW)) {
		*data = this -> m_minpgw;
	}
	else {
		throw ModelException("NutRemv", "GetValue","Parameter " + sk + " does not exist. Please contact the module developer.");
	}
}