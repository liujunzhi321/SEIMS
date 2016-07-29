#include <cmath>
#include "reach.h"
#include "util.h"

reach::reach(void)
{
	m_Vseep = NODATA_VALUE;
	m_VBank1 = NODATA_VALUE;
	m_RchParam = NULL;
	m_Qin = NODATA_VALUE;
	m_TlOutflow = NODATA_VALUE;
	clear();
}


reach::~reach(void)
{
	if(m_RchParam!=NULL) delete[] m_RchParam;
	m_Inlet.clear();
}

void reach::setRchParameter(int id, int RecRchID, int RchOrd, float RchWid, float RchLen, float RchDep, float RchVel, float ContArea, int RchDiv, int RchPs)
{
	this->clear();

	m_id = id;
	m_RecRchID = RecRchID;
	m_RchOrd = RchOrd;
	m_RchWid = RchWid;
	m_RchLen = RchLen;
	m_RchDep = RchDep;
	m_RchVel = RchVel;
	m_ContArea = ContArea;
	m_RchDiv = RchDiv;
	m_RchPs = RchPs;
}

void reach::setRchParameter(float* rchParam)
{
	this->clear();
	m_RchParam = rchParam;

	m_id = int(rchParam[0]);      // id of the reach
	m_RecRchID = int(rchParam[1]);// receiving reach id (downstream reach id)
	m_RchOrd = int(rchParam[2]);  // reach order (shreve ordering)
	m_RchWid = rchParam[3];       // reach width
	m_RchLen = rchParam[4];       // reach length
	m_RchDep = rchParam[5];       // reach depth
	m_RchVel = rchParam[6];       // average velocity
	m_ContArea = rchParam[7];     // contributing area
	m_RchDiv = int(rchParam[8]);  // code of flow diversion of the reach (0 - no diversion, 1 - diversion)
	m_RchPs = int(rchParam[9]);   // code of point source into the reach(0 - no point source, 1 - point source)
}

float* reach::getRchParam()
{
	if(m_RchParam == NULL) throw ModelException("IUH_CH","getRchParam","You have not set the reach parameters.");
	return m_RchParam;
}

void reach::setRchInputs(float QS,float QI,float QG)
{
	//only if you know the upstreams of this reach, you can set the	QS,QI,QG
	m_qsSub = QS;
	m_qiSub = QI;
	m_qgSub = QG;

	m_Qin = QS + QI + QG;     // m3/s
	if (m_RchOrd == 1)
	{
		m_Vin = 0.0f;   // flow from the upstreams (m3)
	}
	else
	{
		m_Vin = m_Qin  * m_dt - m_Vside;   // m3
	}
}

void reach::setRoutedQ(float QS,float QI,float QG)
{
	m_qsSub = QS;
	m_qiSub = QI;
	m_qgSub = QG;
	m_Qin = QS + QI + QG;     // m3/s
	if (m_RchOrd == 1)
	{
		m_Vin = 0.0f;   // flow from the upstreams (m3)
	}
	else
	{
		m_Vin = m_Qin  * m_dt;   // m3
	}
}

void reach::setVside(float QSside, float QIside, float QGside)
{
	m_Vside = (QSside + QIside + QGside) * m_dt;   // m3
}

void reach:: setTlOutFlow(float flowout)
{
	m_TlOutflow = flowout;
}

void reach::update_QS_QI_QG(float Kchb, float Kbank, float Epch, float Bnk0, 
	float Chs0, float Abnk, float Bbnk, float SubArea,float Vdiv, float Vpoint,
	float PETch, float GWSsub, float Vseep0)
{
	// initial value
	if (m_Vseep == NODATA_VALUE) 
		m_Vseep = Vseep0;
	if (m_VBank1 == NODATA_VALUE) 
		m_VBank1 = Bnk0;

	//calculate transmission losses to deep aquifer
	// the unit of kchb is mm/hr
	float seepRate = Kchb/1000.f /3600.f * m_RchWid * m_RchLen;  // m3/s
	if(FloatEqual(m_qgSub,0.0f))
	{
		if (m_Qin > seepRate)
		{
			m_Vseep = seepRate * m_dt;
			m_Qin -= seepRate;
		}
		else
		{
			m_Vseep = m_Qin * m_dt;
			m_Qin = 0.f;
		}
	}

	// calculate transmission losses to bank storage
	if (!FloatEqual(m_Qin, 0.f))
	{
		float Dch = m_Qin / m_RchVel / m_RchWid;			//m
		Dch = min(Dch, m_RchDep);                           //Dch <= RchDep
		float bankInRate = 2 * Kbank/1000.f/3600.f * Dch * m_RchLen ;   // m3/s
		if (m_Qin > bankInRate)
		{
			m_VbankIn = bankInRate * m_dt;
			m_Qin -= bankInRate;
		}
		else
		{
			m_VbankIn = m_Qin * m_dt;
			m_Qin = 0.f;
		}
	}


	// note the m_PETch is the potential evapotranspiration rate (mm)
	m_Ech = Epch * PETch * m_RchWid * m_RchLen / 1000;    //m3

	m_VbankOut = 0.0f;
	m_VbankOut = m_VBank1 * (1 - exp(-Abnk));
	m_Lbank = m_VBank1 * (1 - exp(-Bbnk));
	m_Vbank = m_VBank1 + m_VbankIn - m_VbankOut - m_Lbank;
	m_VBank1 = m_Vbank;
	m_gwStorage = GWSsub + m_VbankOut / SubArea * 1000;   // updated groundwater storage
	m_Vnb = m_VbankOut - m_VbankIn;      //m3
	// Adjust input for channel routing
	m_Vdiv = Vdiv;  // diversion loss
	m_Vtl = m_Vseep - m_Vnb + m_Ech + m_Vdiv;    //total water loss of the river reach
	// Recalculate Qs, Qi and Qg at the reach inlet
	m_qsSub = m_qsSub * (1 - m_Vtl / (m_Vin + m_Vside));
	m_qiSub = m_qiSub * (1 - m_Vtl / (m_Vin + m_Vside));
	m_qgSub = m_qgSub * (1 - m_Vtl / (m_Vin + m_Vside));

	// the point source discharge is assumed to be added to Qs
	m_Vpoint = Vpoint;      //m3
	m_qsSub = m_qsSub + m_Vpoint / m_dt;   

	m_Vinflow = (m_qsSub + m_qiSub + m_qgSub) * m_dt;     //m3
}


float reach::getQG()
{
	return m_qgSub;
}

float reach::getQS()
{
	return m_qsSub;
}

float reach::getQI()
{
	return m_qiSub;
}

float reach::getQin()
{
	return m_Qin;
}

int reach::getId()
{
	return m_id;
}

int reach::getOrder()
{
	return m_RchOrd;
}

int reach::getRecRchID()
{
	return m_RecRchID;
}

void reach::clear()
{
	m_RecRchID = 0;
	m_RchOrd = 0;
	m_RchWid = 0.0f;
	m_RchLen = 0.0f;
	m_RchDep = 0.0f;
	m_RchVel = 0.0f;
	m_ContArea = 0.0f;
	m_RchDiv = 0;
	m_RchPs = 0;
}

bool reach::operator < (const reach & R)const
{
	if(m_RchOrd < R.m_RchOrd) 
		return true;
	if (m_RchOrd == R.m_RchOrd)
	{
		if(m_id < R.m_id) 
			return true;
	}
	return false;
}

float reach::getVinflow()
{
	return m_Vinflow;
}

float reach::getVout()
{
	return m_TlOutflow;
}

float reach::getContArea()
{
	return m_ContArea;
}

float reach::getRchLen()
{
	return m_RchLen;
}

// output
float reach::getVin()
{
	return m_Vin;
}
float reach::getVside()
{
	return m_Vside;
}
float reach::getVdiv()
{
	return m_Vdiv;
}
float reach::getVpoint()
{
	return m_Vpoint;
}
float reach::getVseep()
{
	return m_Vseep;
}
float reach::getVnb()
{
	return m_Vnb;
}
float reach::getEch()
{
	return m_Ech;
}
float reach::getLbank()
{
	return m_Lbank;
}
float reach::getVbank()
{
	return m_Vbank;
}

