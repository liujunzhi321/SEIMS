#pragma once

#include "BMPReach.h"
#include "clsInterpolationWeightData.h"
#include "clsSimpleTxtData.h"

namespace MainBMP
{
    class BMPReachReservoir :
            public BMPReach
    {
    public:
        BMPReachReservoir(
                string bmpDatabasePath,
                string parameterTableName,
                int reachId,
                int ReservoirId);

        ~BMPReachReservoir(void);

        void Dump(ostream *fs);

    private:
        string ColumnName();

        bool HasTimeSeriesData();

        //process different method
        static int FlowRoutingMethod2Int(string method);

        static int SedimentRoutingMethod2Int(string method);

        static int NutrientMethod2Int(string method);

    private:
        /*
        ** Load reservoir parameters from file
        */
        void loadStructureSpecificParameter();

        /*
        ** Get method of flow, seidment and nutrient routing
        ** Called by constructor.
        */
        void getRoutingMethod(string parameterTableName);

    private:
        /*
        ** Rating Curve Data for RAT_RES
        */
        clsInterpolationWeightData *m_ratingCurveData;        //for RAT_RES
        /*
        ** Operation Schedual Data for TRR_RES
        */
        clsSimpleTxtData *m_operationSchedualData;//for TRR_RES
    public:
        clsInterpolationWeightData *RatingCurve();

        clsSimpleTxtData *OperationSchedual();

    private:
        int m_methodFlowInt;
        int m_methodSedimentInt;
        int m_methodNutrientInt;
    public:
        //public function to read all the information
        int MethodFlowRouting() const { return m_methodFlowInt; }

        int MethodSedimentRouting() const { return m_methodSedimentInt; }

        int MethodNutrientRouting() const { return m_methodNutrientInt; }

        float SurfaceAreaEmergency() const { return m_numericParameters[RESERVOIR_SA_EM_INDEX]; }

        float StorageEmergency() const { return m_numericParameters[RESERVOIR_V_EM_INDEX]; }

        float SurfaceAreaPrinciple() const { return m_numericParameters[RESERVOIR_SA_PR_INDEX]; }

        float StoragePrinciple() const { return m_numericParameters[RESERVOIR_V_PR_INDEX]; }

        float StorageInitial() const { return m_numericParameters[RESERVOIR_INI_S_INDEX]; }

        float HydraulicConductivity() const { return m_numericParameters[RESERVOIR_k_res_INDEX]; }

        float *OutFlowMin() const { return &(m_numericParameters[RESERVOIR_OFLOWMN01_INDEX]); }

        float *OutFlowMax() const { return &(m_numericParameters[RESERVOIR_OFLOWMX01_INDEX]); }

        float ReleaseRatePrinciple() const { return m_numericParameters[RESERVOIR_Q_REL_INDEX]; }

        float SedimentConEquilibrium() const { return m_numericParameters[RESERVOIR_NSED_INDEX]; }

        float SedimentConInitial() const { return m_numericParameters[RESERVOIR_INI_SC_INDEX]; }

        float D50() const
        {
            float d = m_numericParameters[RESERVOIR_D50_INDEX];
            if (d < 0) d = 10.0f;
            return d;
        }
    };
}



