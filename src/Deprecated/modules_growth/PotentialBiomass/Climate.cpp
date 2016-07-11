#include "Climate.h"
#include <math.h>
#include <stdlib.h>
#include "util.h"

Climate::Climate()
{
}

Climate::~Climate()
{
}

void Climate::SetClimate(float tMin, float tMax, float PET, float soilET, float sr, float rhd, float co2)
{
    m_averageTemperature = tMin / 2.0f + tMax / 2.0f;
    m_PET = PET;
    m_solarRadiation = sr;
    m_relativeHumidity = rhd;
    m_co2 = co2;
    m_soilET = soilET;
}

float Climate::AverageTemperature(void) const
{
    return m_averageTemperature;
}

//float Climate::MaximumPlantEvapotranpiration(IPlant* plant) const
//{
//	float ep_max = 0.0f;
//	if(m_PET > 1.0e-6)
//	{
//		if(plant->LAI() <= 3.0f)
//			ep_max = plant->LAI() * m_PET / 3.0f;
//		else
//			ep_max = m_PET;
//	}
//	if(ep_max < 0) ep_max = 0.0f;
//	return ep_max;
//}

float Climate::SolarRadiation() const
{
    return m_solarRadiation;
}

float Climate::CO2() const
{
    return m_co2;
}

float Climate::VaporPressureDeficit() const
{
    return CalSaturationVaporPressure(m_averageTemperature) * (1.0f - m_relativeHumidity);
}

float Climate::CalSaturationVaporPressure(float t)
{
    if (abs(t + 237.3f) >= 1.0e-6f)
    {
        float ee = (16.78f * t - 116.9f) / (t + 237.3f);
        return exp(ee);
    }
    return 0.0f;
}

float Climate::PET(void) const
{
    return m_PET;
}

float Climate::SoilET(void) const
{
    return m_soilET;
}
