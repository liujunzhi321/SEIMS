/*!
 * \file PlantManagementOperation.h
 * \brief
 * \author Liang-Jun Zhu
 * \date June 2016
 */
#pragma once

#include <string>
#include <vector>

using namespace std;
namespace MainBMP
{
    /*!
     *  \addtogroup PlantManagement
     * \ingroup MainBMP
     * \brief Plant management related operations
     */
    namespace PlantManagement
    {
        /*!
         * \class PlantManagementOperation
         * \ingroup PlantManagement
         * \brief Base class of plant management operation
         */
        class PlantManagementOperation
        {
        public:
            /*!
             * \brief Constructor
             * \param[in] mgtCode 1 to 16
             * \param[in] husc Fraction of heat units (base or plant)
             * \param[in] year Rotation year, e.g., 1,2,...
             * \param[in] month
             * \param[in] day
             * \param[in] location Indexes of field to practice the operation (string)
             */
            PlantManagementOperation(int mgtOp, float husc, int year, int month, int day, float *parameters);

            //! Destructor
            ~PlantManagementOperation(void);

            //! Output
            virtual void dump(ostream *fs) = 0;

            float &GetHUFraction() { return m_frHU; }

            int &GetMonth() { return m_month; }

            int &GetDay() { return m_day; }

            int &GetOperationCode() { return m_mgtOp; }

        protected:
            /// husc
            float m_frHU;
            /// year
            int m_year;
            /// month
            int m_month;
            /// day
            int m_day;
            /// management operation code
            int m_mgtOp;
            /// parameters, mgt1~mgt10
            float *m_parameters;
        };

        /*!
         * \class PlantOperation
         * \ingroup PlantManagement
         * \brief Plant management operation
         */
        class PlantOperation : public PlantManagementOperation
        {
        public:
            PlantOperation(int mgtOp, float husc, int year, int month, int day, float *parameters);

            ~PlantOperation();

            int PlantID() { return m_plantID; }

            float CurYearMaturity() { return m_curYrMat; }

            float HeatUnits() { return m_heatUnits; }

            float LAIInit() { return m_laiInit; }

            float BIOInit() { return m_bioInit; }

            float HITarg() { return m_hiTarg; }

            float BIOTarg() { return m_bioTarg; }

            float CNOP() { return m_CNOP; }

            //! Output
            void dump(ostream *fs);

        private:
            int m_plantID;
            float m_curYrMat;
            float m_heatUnits;
            float m_laiInit;
            float m_bioInit;
            float m_hiTarg;
            float m_bioTarg;
            float m_CNOP;
        };

        /*!
         * \class IrrigationOperation
         * \ingroup PlantManagement
         * \brief Irrigation operation
         */
        class IrrigationOperation : public PlantManagementOperation
        {
        public:
            IrrigationOperation(int mgtOp, float husc, int year, int month, int day, float *parameters);

            ~IrrigationOperation();

            int IRRSource() { return m_irrSrc; }

            int IRRNo() { return m_irrNo; }

            float IRRApplyDepth() { return m_irrAmt; }

            float IRRSalt() { return m_irrSalt; }

            float IRREfficiency() { return m_irrEfm; }

            float IRRSQfrac() { return m_irrSq; }

            //! Output
            void dump(ostream *fs);

        private:
            int m_irrSrc;
            float m_irrAmt;
            float m_irrSalt;
            float m_irrEfm;
            float m_irrSq;
            int m_irrNo;
        };

        /*!
         * \class FertilizerOperation
         * \ingroup PlantManagement
         * \brief Fertilizer operation
         */
        class FertilizerOperation : public PlantManagementOperation
        {
        public:
            FertilizerOperation(int mgtOp, float husc, int year, int month, int day, float *parameters);

            ~FertilizerOperation();

            int FertilizerID() { return m_fertID; }

            float FertilizerKg() { return m_frtKg; }

            float FertilizerSurfaceFrac() { return m_frtSurface; }

            //! Output
            void dump(ostream *fs);

        private:
            int m_fertID;
            float m_frtKg;
            float m_frtSurface;
        };

        /*!
         * \class PesticideOperation
         * \ingroup PlantManagement
         * \brief Pesticide operation
         */
        class PesticideOperation : public PlantManagementOperation
        {
        public:
            PesticideOperation(int mgtOp, float husc, int year, int month, int day, float *parameters);

            ~PesticideOperation();

            int PesticideID() { return m_pestID; }

            float PesticideKg() { return m_pstKg; }

            float PesticideDepth() { return m_pstDep; }

            //! Output
            void dump(ostream *fs);

        private:
            int m_pestID;
            float m_pstKg;
            float m_pstDep;
        };

        /*!
         * \class HarvestKillOperation
         * \ingroup PlantManagement
         * \brief HarvestKill operation
         */
        class HarvestKillOperation : public PlantManagementOperation
        {
        public:
            HarvestKillOperation(int mgtOp, float husc, int year, int month, int day, float *parameters);

            ~HarvestKillOperation();

            float CNOP() { return m_CNOP; }

            float HarvestIndexOverride() { return m_hiOvr; }

            float StoverFracRemoved() { return m_fracHarvk; }

            //! Output
            void dump(ostream *fs);

        private:
            float m_CNOP;
            float m_hiOvr;
            float m_fracHarvk;
        };

        /*!
         * \class TillageOperation
         * \ingroup PlantManagement
         * \brief Tillage operation
         */
        class TillageOperation : public PlantManagementOperation
        {
        public:
            TillageOperation(int mgtOp, float husc, int year, int month, int day, float *parameters);

            ~TillageOperation();

            float CNOP() { return m_CNOP; }

            int TillageID() { return m_tillID; }

            //! Output
            void dump(ostream *fs);

        private:
            int m_tillID;
            float m_CNOP;
        };

        /*!
         * \class HarvestOnlyOperation
         * \ingroup PlantManagement
         * \brief HarvestOnly operation
         */
        class HarvestOnlyOperation : public PlantManagementOperation
        {
        public:
            HarvestOnlyOperation(int mgtOp, float husc, int year, int month, int day, float *parameters);

            ~HarvestOnlyOperation();

            float HarvestEfficiency() { return m_harvEff; }

            float HarvestIndexResidue() { return m_hiRsd; }

            float HarvestIndexBiomass() { return m_hiBms; }

            //! Output
            void dump(ostream *fs);

        private:
            float m_harvEff;
            float m_hiRsd;
            float m_hiBms;
        };

        /*!
         * \class KillOperation
         * \ingroup PlantManagement
         * \brief Kill operation
         */
        class KillOperation : public PlantManagementOperation
        {
        public:
            KillOperation(int mgtOp, float husc, int year, int month, int day, float *parameters);

            ~KillOperation();

            //! Output
            void dump(ostream *fs);
        };

        /*!
         * \class GrazingOperation
         * \ingroup PlantManagement
         * \brief Grazing operation
         */
        class GrazingOperation : public PlantManagementOperation
        {
        public:
            GrazingOperation(int mgtOp, float husc, int year, int month, int day, float *parameters);

            ~GrazingOperation();

            int GrazingDays() { return m_grzDays; }

            int ManureID() { return m_manureID; }

            float BiomassConsumed() { return m_bioEat; }

            float BiomassTrampled() { return m_bioTrmp; }

            float ManureDeposited() { return m_manureKg; }

            //! Output
            void dump(ostream *fs);

        private:
            int m_grzDays;
            int m_manureID;
            float m_bioEat;
            float m_bioTrmp;
            float m_manureKg;
        };

        /*!
         * \class AutoIrrigationOperation
         * \ingroup PlantManagement
         * \brief Auto irrigation operation
         */
        class AutoIrrigationOperation : public PlantManagementOperation
        {
        public:
            AutoIrrigationOperation(int mgtOp, float husc, int year, int month, int day, float *parameters);

            ~AutoIrrigationOperation();

            int WaterStrsIdent() { return m_wstrsID; }

            int AutoIrrSrcCode() { return m_irrSrc; }

            int AutoIrrSrcLocs() { return m_irrNoa; }

            float AutoWtrStrsThrsd() { return m_autoWstrs; }

            float IrrigationEfficiency() { return m_irrEff; }

            float IrrigationWaterApplied() { return m_irrMx; }

            float SurfaceRunoffRatio() { return m_irrAsq; }

            //! Output
            void dump(ostream *fs);

        private:
            int m_wstrsID;
            int m_irrSrc;
            float m_autoWstrs;
            int m_irrNoa;
            float m_irrEff;
            float m_irrMx;
            float m_irrAsq;
        };

        /*!
         * \class AutoFertilizerOperation
         * \ingroup PlantManagement
         * \brief Auto Fertilizer operation
         */
        class AutoFertilizerOperation : public PlantManagementOperation
        {
        public:
            AutoFertilizerOperation(int mgtOp, float husc, int year, int month, int day, float *parameters);

            ~AutoFertilizerOperation();

            int FertilizerID() { return m_afertID; }

            int NitrogenMethod() { return m_NStress; }

            float NitrogenStrsFactor() { return m_autoNStrs; }

            float MaxMineralN() { return m_autoNAPP; }

            float MaxMineralNYearly() { return m_autoNYR; }

            float FertEfficiency() { return m_autoEff; }

            float SurfaceFracApplied() { return m_afrtSurface; }

            //! Output
            void dump(ostream *fs);

        private:
            int m_afertID;
            int m_NStress;
            float m_autoNStrs;
            float m_autoNAPP;
            float m_autoNYR;
            float m_autoEff;
            float m_afrtSurface;
        };

        /*!
         * \class ReleaseImpoundOperation
         * \ingroup PlantManagement
         * \brief ReleaseImpound operation
         */
        class ReleaseImpoundOperation : public PlantManagementOperation
        {
        public:
            ReleaseImpoundOperation(int mgtOp, float husc, int year, int month, int day, float *parameters);

            ~ReleaseImpoundOperation();

            int ImpoundTriger() { return m_impTrig; }

            //! Output
            void dump(ostream *fs);

        private:
            int m_impTrig;
        };

        /*!
         * \class ContinuousFertilizerOperation
         * \ingroup PlantManagement
         * \brief Continuous Fertilizer operation
         */
        class ContinuousFertilizerOperation : public PlantManagementOperation
        {
        public:
            ContinuousFertilizerOperation(int mgtOp, float husc, int year, int month, int day, float *parameters);

            ~ContinuousFertilizerOperation();

            int FertilizerID() { return m_cfertID; }

            int ApplyFrequency() { return m_ifrtFreq; }

            int FertilizerDays() { return m_fertDays; }

            float FertilizerKg() { return m_cfrtKg; }

            //! Output
            void dump(ostream *fs);

        private:
            int m_cfertID;
            int m_fertDays;
            float m_cfrtKg;
            int m_ifrtFreq;
        };

        /*!
         * \class ContinuousPesticideOperation
         * \ingroup PlantManagement
         * \brief Continuous Pesticide operation
         */
        class ContinuousPesticideOperation : public PlantManagementOperation
        {
        public:
            ContinuousPesticideOperation(int mgtOp, float husc, int year, int month, int day, float *parameters);

            ~ContinuousPesticideOperation();

            int PesticideID() { return m_ipstID; }

            int PesticideFrequency() { return m_pstFreq; }

            float PesticideKg() { return m_cpstKg; }

            int PesticideDays() { return m_pstDays; }

            //! Output
            void dump(ostream *fs);

        private:
            int m_ipstID;
            int m_pstDays;
            float m_cpstKg;
            int m_pstFreq;
        };

        /*!
         * \class BurningOperation
         * \ingroup PlantManagement
         * \brief Burning operation
         */
        class BurningOperation : public PlantManagementOperation
        {
        public:
            BurningOperation(int mgtOp, float husc, int year, int month, int day, float *parameters);

            ~BurningOperation();

            float FractionLeft() { return m_burnFrlb; }

            //! Output
            void dump(ostream *fs);

        private:
            float m_burnFrlb;
        };
    }
}



