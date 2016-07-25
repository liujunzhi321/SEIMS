using namespace std;
#ifndef CLIMATE_H
#define CLIMATE_H


class Climate
{
public:
    Climate(void);

    ~Climate(void);

    //put climate data into this container
    void SetClimate(float tMin, float tMax, float PET, float soilET, float sr, float rhd, float co2);

private:
    float m_averageTemperature;
    float m_solarRadiation;
    float m_co2;
    float m_PET;
    float m_relativeHumidity;
    float m_soilET;
public:
    inline float AverageTemperature(void) const;

    inline float VaporPressureDeficit(void) const;

    //inline float MaximumPlantEvapotranpiration(IPlant*) const;
    inline float SolarRadiation(void) const;

    inline float CO2(void) const;

    inline float PET(void) const;

    inline float SoilET(void) const;

public:
    //calculate saturation vapor pressure for given temperature
    static float CalSaturationVaporPressure(float t);
};

#endif