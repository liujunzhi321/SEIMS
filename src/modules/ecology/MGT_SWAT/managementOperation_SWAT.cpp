#include "api.h"
#include <iostream>
#include "managementOperation_SWAT.h"
#include "PlantGrowthCommon.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"

using namespace std;

MGTOpt_SWAT::MGTOpt_SWAT(void) : m_scenario(NULL), m_nCells(-1), m_nSub(-1), m_cellWidth(NODATA_VALUE), m_cellArea(NODATA_VALUE),
        /// add parameters from MongoDB
                                 m_subBsnID(NULL), m_landUse(NULL), m_landCover(NULL), m_mgtFields(NULL),
        /// Soil related parameters from MongoDB
                                 m_nSoilLayers(NULL), m_soilDepth(NULL), m_soilThick(NULL), m_soilZMX(NULL),
                                 m_soilBD(NULL), m_soilSumFC(NULL), m_soilN(NULL),
                                 m_soilCarbon(NULL), m_soilRock(NULL), m_soilClay(NULL), m_soilSand(NULL),
                                 m_soilSilt(NULL),
        /// Soil related parameters -- both as inputs and outputs
                                 m_soilActiveOrgN(NULL), m_soilFreshOrgN(NULL), m_soilFreshOrgP(NULL), m_soilNH3(NULL),
                                 m_soilNO3(NULL), m_soilStableOrgN(NULL),
                                 m_soilOrgP(NULL), m_soilSolP(NULL),
        /// Plant operation related parameters
                                 m_landuseLookup(NULL), m_landuseNum(-1), m_CN2(NULL), m_igro(NULL),
                                 m_HarvestIdxTarg(NULL), m_BiomassTarg(NULL), m_curYearMat(NULL),
                                 m_wtrStrsYF(NULL), m_LAIDay(NULL), m_phuBase(NULL), m_phuAcc(NULL), m_phuPlant(NULL),
                                 m_dormFlag(NULL), m_havstIdx(NULL), m_havstIdxAdj(NULL),
                                 m_LAIMaxFr(NULL), m_oLAI(NULL), m_frPlantN(NULL), m_plantN(NULL), m_plantP(NULL),
                                 m_pltET(NULL), m_pltPET(NULL), m_frRoot(NULL), m_biomass(NULL),
        /// Harvest and Kill operation
                                 m_soilRsd(NULL), m_frStrsWa(NULL), m_cropLookup(NULL), m_cropNum(-1),
                                 m_lastSoilRootDepth(NULL),
        /// Fertilizer operation
                                 m_fertilizerLookup(NULL), m_fertilizerNum(-1), m_cswat(0),
        /// Irrigation
                                 m_irrFlag(NULL), m_appliedWater(NULL), m_irrSurfQWater(NULL), m_deepWaterDepth(NULL),
                                 m_shallowWaterDepth(NULL), m_impoundVolume(NODATA_VALUE),
                                 m_impoundArea(NODATA_VALUE), m_deepIrrWater(NULL), m_shallowIrrWater(NULL),
        /// auto irrigation operation
                                 m_wtrStrsID(NULL), m_autoWtrStres(NULL), m_autoIrrSource(NULL), m_autoIrrNo(NULL),
                                 m_autoIrrEfficiency(NULL),
                                 m_autoIrrWtrDepth(NULL), m_autoSurfRunRatio(NULL),
        /// bacteria related
        //m_bactSwf(NODATA),	m_bactLessPersistPlt(NULL), m_bactLessPersistSol(NULL), m_bactLessPersistParticle(NULL),
        //m_bactPersistPlt(NULL), m_bactPersistSol(NULL), m_bactPersistParticle(NULL),
        /// Tillage operation
                                 m_tillageLookup(NULL), m_soilActiveMinP(NULL), m_soilStableMinP(NULL),
        /// auto fertilizer operation
                                 m_fertilizerID(NULL), m_NStressCode(NULL), m_autoNStress(NULL),
                                 m_autoMaxAppliedN(NULL), m_autoAnnMaxAppliedMinN(NULL),
                                 m_targNYld(NULL), m_autoFertEfficiency(NULL), m_autoFertSurface(NULL),
        /// Grazing operation
                                 m_nGrazingDays(NULL), m_grzFlag(NULL),
        /// Release or impound operation
                                 m_impoundTriger(NULL),
        /// Temporary parameters
                                 m_doneOpSequence(NULL)
{
}


MGTOpt_SWAT::~MGTOpt_SWAT(void)
{
    /// release map containers
    if (m_scenario != NULL) delete m_scenario;
    if (!m_mgtFactory.empty())
    {
        for (map<int, BMPPlantMgtFactory *>::iterator it = m_mgtFactory.begin(); it != m_mgtFactory.end(); it++)
        {
            if (it->second != NULL) delete it->second;
        }
    }
    /// release output parameters

}

void MGTOpt_SWAT::SetValue(const char *key, float data)
{
    string sk(key);
    if (StringMatch(sk, VAR_OMP_THREADNUM)) omp_set_num_threads((int) data);
    else if (StringMatch(sk, VAR_CSWAT)) m_cswat = (int) data;
    else if (StringMatch(sk, Tag_CellWidth)) m_cellWidth = data;
    else
        throw ModelException(MID_MGT_SWAT, "SetValue", "Parameter " + sk
                                                       +
                                                       " does not exist in current module. Please contact the module developer.");
}

bool MGTOpt_SWAT::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
        throw ModelException(MID_MGT_SWAT, "CheckInputSize", "Input data for " + string(key) +
                                                             " is invalid. The size could not be less than zero.");
    if (m_nCells != n)
    {
        if (m_nCells <= 0)
            m_nCells = n;
        else
        {
            throw ModelException(MID_MGT_SWAT, "CheckInputSize", "Input data for " + string(key) +
                                                                 " is invalid. All the input raster data should have same size.");
            return false;
        }
    }
    return true;
}

void MGTOpt_SWAT::Set1DData(const char *key, int n, float *data)
{
    string sk(key);
    CheckInputSize(key, n);
    //// /// add parameters from MongoDB
    if (StringMatch(sk, VAR_SUBBSN)) m_subBsnID = data;
    else if (StringMatch(sk, VAR_LANDUSE)) m_landUse = data;
    else if (StringMatch(sk, VAR_LANDCOVER)) m_landCover = data;
    else if (StringMatch(sk, VAR_MGT_FIELD)) m_mgtFields = data;
        /// Soil related parameters from MongoDB
    else if (StringMatch(sk, VAR_SOILLAYERS)) m_nSoilLayers = data;
    else if (StringMatch(sk, VAR_SOL_ZMX)) m_soilZMX = data;
    else if (StringMatch(sk, VAR_SOL_SUMAWC)) m_soilSumFC = data;
        ///  Plant operation related parameters
    else if (StringMatch(sk, VAR_CN2)) m_CN2 = data;
    else if (StringMatch(sk, VAR_HVSTI)) m_havstIdx = data;
    else if (StringMatch(sk, VAR_WSYF)) m_wtrStrsYF = data;
    else if (StringMatch(sk, VAR_PHUPLT)) m_phuPlant = data;
    else if (StringMatch(sk, VAR_IGRO)) m_igro = data;
    else if (StringMatch(sk, VAR_FR_PHU_ACC)) m_phuAcc = data;
    else if (StringMatch(sk, VAR_TREEYRS)) m_curYearMat = data;
    else if (StringMatch(sk, VAR_HVSTI_ADJ)) m_havstIdxAdj = data;
    else if (StringMatch(sk, VAR_LAIDAY)) m_LAIDay = data;
    else if (StringMatch(sk, VAR_DORMI)) m_dormFlag = data;
    else if (StringMatch(sk, VAR_LAIMAXFR)) m_LAIMaxFr = data;
    else if (StringMatch(sk, VAR_OLAI)) m_oLAI = data;
    else if (StringMatch(sk, VAR_PLANT_N)) m_plantN = data;
    else if (StringMatch(sk, VAR_PLANT_P)) m_plantP = data;
    else if (StringMatch(sk, VAR_FR_PLANT_N)) m_frPlantN = data;
    else if (StringMatch(sk, VAR_FR_PLANT_P)) m_frPlantP = data;
    else if (StringMatch(sk, VAR_PLTET_TOT)) m_pltET = data;
    else if (StringMatch(sk, VAR_PLTPET_TOT)) m_pltPET = data;
    else if (StringMatch(sk, VAR_FR_ROOT)) m_frRoot = data;
    else if (StringMatch(sk, VAR_BIOMASS)) m_biomass = data;
        //// Harvest and Kill operation
    else if (StringMatch(sk, VAR_LAST_SOILRD)) m_lastSoilRootDepth = data;
        /// Irrigation operation
    else if (StringMatch(sk, VAR_FR_STRSWTR)) m_frStrsWa = data;
    else if (StringMatch(sk, VAR_DEEPST)) m_deepWaterDepth = data;
    else if (StringMatch(sk, VAR_SHALLST)) m_shallowWaterDepth = data;
        /// inputs from other modules
    else if (StringMatch(sk, VAR_PHUBASE)) m_phuBase = data;
    else
        throw ModelException(MID_MGT_SWAT, "Set1DData", "Parameter " + sk +
                                                        " does not exist in current module. Please contact the module developer.");
}

bool MGTOpt_SWAT::CheckInputSize2D(const char *key, int n, int col)
{
    CheckInputSize(key, n);
    if (col <= 0)
        throw ModelException(MID_MGT_SWAT, "CheckInputSize2D", "Input data for " + string(key) +
                                                               " is invalid. The layer number could not be less than zero.");
    if (m_soilLayers != col)
    {
        if (m_soilLayers <= 0)
            m_soilLayers = col;
        else
        {
            throw ModelException(MID_MGT_SWAT, "CheckInputSize2D", "Input data for " + string(key) +
                                                                   " is invalid. All the layers of input 2D raster data should have same size.");
            return false;
        }
    }
    return true;
}

void MGTOpt_SWAT::Set2DData(const char *key, int n, int col, float **data)
{
    string sk(key);
    /// lookup tables
    if (StringMatch(sk, VAR_LANDUSE_LOOKUP))
    {
        m_landuseLookup = data;
        m_landuseNum = n;
        if (col != (int) LANDUSE_PARAM_COUNT)
            throw ModelException(MID_MGT_SWAT, "ReadLanduseLookup", "The field number " + ValueToString(col) +
                                                                    "is not coincident with LANDUSE_PARAM_COUNT: " +
                                                                    ValueToString((int) LANDUSE_PARAM_COUNT));
        return;
    }
    else if (StringMatch(sk, VAR_CROP_LOOKUP))
    {
        m_cropLookup = data;
        m_cropNum = n;
        if (col != (int) CROP_PARAM_COUNT)
            throw ModelException(MID_MGT_SWAT, "ReadCropLookup", "The field number " + ValueToString(col) +
                                                                 "is not coincident with CROP_PARAM_COUNT: " +
                                                                 ValueToString((int) CROP_PARAM_COUNT));
        return;
    }
    else if (StringMatch(sk, VAR_FERTILIZER_LOOKUP))
    {
        m_fertilizerLookup = data;
        m_fertilizerNum = n;
        if (col != (int) FERTILIZER_PARAM_COUNT)
            throw ModelException(MID_MGT_SWAT, "ReadFertilizerLookup", "The field number " + ValueToString(col) +
                                                                       "is not coincident with FERTILIZER_PARAM_COUNT: " +
                                                                       ValueToString((int) FERTILIZER_PARAM_COUNT));
        return;
    }
    else if (StringMatch(sk, VAR_TILLAGE_LOOKUP))
    {
        m_tillageLookup = data;
        m_tillageNum = n;
        if (col != (int) TILLAGE_PARAM_COUNT)
            throw ModelException(MID_MGT_SWAT, "ReadTillageLookup", "The field number " + ValueToString(col) +
                                                                    "is not coincident with TILLAGE_PARAM_COUNT: " +
                                                                    ValueToString((int) TILLAGE_PARAM_COUNT));
        return;
    }
    /// 2D raster data
    CheckInputSize2D(key, n, col);
    /// Soil related parameters from MongoDB
    if (StringMatch(sk, VAR_SOILDEPTH)) m_soilDepth = data;
    else if (StringMatch(sk, VAR_SOILTHICK)) m_soilThick = data;
    else if (StringMatch(sk, VAR_SOL_BD)) m_soilBD = data;
    else if (StringMatch(sk, VAR_SOL_CBN)) m_soilCarbon = data;
    else if (StringMatch(sk, VAR_SOL_N)) m_soilN = data;
    else if (StringMatch(sk, VAR_CLAY)) m_soilClay = data;
    else if (StringMatch(sk, VAR_SILT)) m_soilSilt = data;
    else if (StringMatch(sk, VAR_SAND)) m_soilSand = data;
    else if (StringMatch(sk, VAR_ROCK)) m_soilRock = data;
        /// Soil related parameters --  inputs from other modules
    else if (StringMatch(sk, VAR_SOL_AORGN)) m_soilActiveOrgN = data;
    else if (StringMatch(sk, VAR_SOL_FON)) m_soilFreshOrgN = data;
    else if (StringMatch(sk, VAR_SOL_FOP)) m_soilFreshOrgP = data;
    else if (StringMatch(sk, VAR_SOL_ORGN)) m_soilStableOrgN = data;
    else if (StringMatch(sk, VAR_SOL_ORGP)) m_soilOrgP = data;
    else if (StringMatch(sk, VAR_SOL_SOLP)) m_soilSolP = data;
    else if (StringMatch(sk, VAR_SOL_NH3)) m_soilNH3 = data;
    else if (StringMatch(sk, VAR_SOL_NO3)) m_soilNO3 = data;
    else if (StringMatch(sk, VAR_SOL_ACTP)) m_soilActiveMinP = data;
    else if (StringMatch(sk, VAR_SOL_STAP)) m_soilStableMinP = data;
    else if (StringMatch(sk, VAR_SOL_RSD)) m_soilRsd = data;
    else
        throw ModelException(MID_MGT_SWAT, "Set2DData", "Parameter " + sk +
                                                        " does not exist in current module. Please contact the module developer.");
}

void MGTOpt_SWAT::SetScenario(Scenario *sce)
{
    if (NULL != sce)
        m_scenario = sce;
    map<int, BMPFactory *> tmpBMPFactories = m_scenario->GetBMPFactories();
    if (!m_mgtFactory.empty())
        m_mgtFactory.clear();
    for (map<int, BMPFactory *>::iterator it = tmpBMPFactories.begin(); it != tmpBMPFactories.end(); it++)
    {
        /// Key is uniqueBMPID, which is calculated by Landuse_ID * 100 + subScenario;
        if (it->first / 100 == BMP_TYPE_PLANT_MGT)
        {
            int uniqueIdx = ((BMPPlantMgtFactory *) it->second)->GetLUCCID() * 100 + it->second->GetSubScenarioId();
            m_mgtFactory[uniqueIdx] = (BMPPlantMgtFactory *) it->second;
        }
    }
}

bool MGTOpt_SWAT::CheckInputData(void)
{
    // DT_Single
    if (m_date <= 0) throw ModelException(MID_MGT_SWAT, "CheckInputData", "You have not set the time.");
    if (m_nCells <= 0)
        throw ModelException(MID_MGT_SWAT, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    if (m_cellWidth <= 0)
        throw ModelException(MID_MGT_SWAT, "CheckInputData",
                             "The cell width of the input data can not be less than zero.");
    if (m_soilLayers <= 0)
        throw ModelException(MID_MGT_SWAT, "CheckInputData",
                             "The layer number of the input 2D raster data can not be less than zero.");
    if (m_cswat < 0) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Carbon modeling method must be 0, 1, or 2");
    /// DT_Raster
    if (m_subBsnID == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "SubBasin ID must not be NULL");
    if (m_landUse == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Landuse must not be NULL");
    if (m_landCover == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Landcover must not be NULL");
    if (m_mgtFields == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Management fields must not be NULL");
    if (m_nSoilLayers == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil layeres number must not be NULL");
    if (m_soilZMX == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Maximum soil root depth must not be NULL");
    if (m_soilSumFC == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Summary amount water in field capacity must not be NULL");
    if (m_CN2 == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "CN2 value must not be NULL");
    if (m_igro == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Plant growth code must not be NULL");
    if (m_curYearMat == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Current growth year must not be NULL");
    if (m_wtrStrsYF == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Value of harvest index must not be NULL");
    if (m_LAIDay == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "LAI in current day must not be NULL");
    if (m_phuBase == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Base heat units fraction must not be NULL");
    if (m_phuAcc == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Accumulated heat units fraction must not be NULL");
    if (m_phuPlant == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Heat units needed by plant to maturity must not be NULL");
    if (m_dormFlag == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Dormancy flag must not be NULL");
    if (m_havstIdx == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Harvest index must not be NULL");
    if (m_havstIdxAdj == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Adjusted harvest index must not be NULL");
    if (m_LAIMaxFr == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "LAI maximum fraction must not be NULL");
    if (m_oLAI == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "oLAI must not be NULL");
    if (m_frPlantN == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Fraction of biomass in nitrogen must not be NULL");
    if (m_frPlantP == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Fraction of biomass in phosphorus must not be NULL");
    if (m_pltET == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData",
                             "Actual ET simulated during life of plant must not be NULL");
    if (m_pltPET == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData",
                             "Potential ET simulated during life of plant must not be NULL");
    if (m_frRoot == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Fraction of total biomass in roots must not be NULL");
    if (m_biomass == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Biomass must not be NULL");
    if (m_lastSoilRootDepth == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Last root depth in soil must not be NULL");
    if (m_deepWaterDepth == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Depth of water in deep aquifer must not be NULL");
    if (m_shallowWaterDepth == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Depth of water in shallow aquifer must not be NULL");
    /// DT_Raster2D
    if (m_soilDepth == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil depth must not be NULL");
    if (m_soilThick == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil thickness must not be NULL");
    if (m_soilBD == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil bulk density must not be NULL");
    if (m_soilN == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil nitrogen must not be NULL");
    if (m_soilCarbon == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil carbon content must not be NULL");
    if (m_soilRock == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil rock content must not be NULL");
    if (m_soilClay == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil clay content must not be NULL");
    if (m_soilSand == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil sand content must not be NULL");
    if (m_soilSilt == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil silt content must not be NULL");
    if (m_soilActiveOrgN == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil active organic N must not be NULL");
    if (m_soilFreshOrgN == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil fresh organic N must not be NULL");
    if (m_soilFreshOrgP == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil fresh organic P must not be NULL");
    if (m_soilNH3 == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil NH3 must not be NULL");
    if (m_soilNO3 == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil NO3 must not be NULL");
    if (m_soilStableOrgN == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil stable organic N must not be NULL");
    if (m_soilOrgP == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil organic P must not be NULL");
    if (m_soilSolP == NULL) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil soluble P must not be NULL");
    if (m_soilActiveMinP == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil active mineral P must not be NULL");
    if (m_soilStableMinP == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Soil stable mineral P must not be NULL");
    if (m_soilRsd == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData",
                             "OM classified as residue in soil layers must not be NULL");
    return true;
}

bool MGTOpt_SWAT::GetOperationCode(int i, int &factoryID, vector<int> &nOps)
{
    if (m_mgtFactory.empty()) return false;
    int curLanduseID = int(m_landUse[i]);
    int curMgtField = int(m_mgtFields[i]);
    factoryID = -1;
    /// 1. Is there any plant management operations are suitable to current cell.
    for (map<int, BMPPlantMgtFactory *>::iterator it = m_mgtFactory.begin(); it != m_mgtFactory.end(); it++)
    {
        /// Key is uniqueBMPID, which is calculated by Landuse_ID * 100 + subScenario;
        if (curLanduseID == (it->first) / 100)
        {
            /// 2. If current cell located in the locations of this BMPPlantMgtFactory
            if (ValueInVector(curMgtField, it->second->GetLocations()))
            {
                factoryID = it->first;
                break;
            }
            // replaced by LJ, 2016-7-5
            //vector<int> tmpLocations = it->second->GetLocations();
            //vector<int>::iterator findIter = find(tmpLocations.begin(), tmpLocations.end(), curMgtField);
            //if(findIter != tmpLocations.end())
            //{
            //	factoryID = it->first;
            //	break;
            //}
        }
    }
    if (factoryID < 0) return false;
    /// 3. Figure out if any management operation should be applied, i.e., find sequence IDs (nOps)
    vector<int> tmpOpSeqences = m_mgtFactory[factoryID]->GetOperationSequence();
    map<int, PlantManagementOperation *> tmpOperations = m_mgtFactory[factoryID]->GetOperations();
    for (vector<int>::iterator seqIter = tmpOpSeqences.begin(); seqIter != tmpOpSeqences.end(); seqIter++)
    {
        /// *seqIter is calculated by: index * 1000 + operationCode
        bool dateDepent = false, huscDepent = false;
        /// If operation applied date (month and day) are defined
        if (tmpOperations[*seqIter]->GetMonth() != 0 && tmpOperations[*seqIter]->GetDay() != 0)
        {
            struct tm dateInfo;
            LocalTime(m_date, &dateInfo);
            if (dateInfo.tm_mon == tmpOperations[*seqIter]->GetMonth() &&
                dateInfo.tm_mday == tmpOperations[*seqIter]->GetDay())
                dateDepent = true;
        }
        /// If husc is defined
        if (tmpOperations[*seqIter]->GetHUFraction() >= 0.f)
        {
            float aphu; /// fraction of total heat units accumulated
            if (FloatEqual(m_igro[i], 0.))
                aphu = m_phuBase[i];
            else
                aphu = m_phuAcc[i];
            if (m_dormFlag[i] == 1)
                aphu = NODATA_VALUE;
            if (aphu >= tmpOperations[*seqIter]->GetHUFraction())
                huscDepent = true;
        }
        /// The operation will be applied either date or HUSC are satisfied,
        /// and also in case of repeated run
        if ((dateDepent || huscDepent) && *seqIter > m_doneOpSequence[i])
        {
            nOps.push_back(*seqIter);
            m_doneOpSequence[i] = *seqIter; /// update value
        }
    }
    if (nOps.empty()) return false;
    return true;
}

void MGTOpt_SWAT::initializeLanduseLookup()
{
    /// Check input data
    if (m_landuseLookup == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Landuse lookup array must not be NULL");
    if (m_landuseNum <= 0)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Landuse number must be greater than 0");

    if (m_landuseLookup != NULL && m_landuseNum > 0)
    {
        if (!m_landuseLookupMap.empty())
            m_landuseLookupMap.clear();
        for (int i = 0; i < m_landuseNum; i++)
            m_landuseLookupMap[(int) m_landuseLookup[i][0]] = m_landuseLookup[i];
    }
}

void MGTOpt_SWAT::initializeCropLookup()
{
    /// Check input data
    if (m_cropLookup == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Crop lookup array must not be NULL");
    if (m_cropNum <= 0) throw ModelException(MID_MGT_SWAT, "CheckInputData", "Crop number must be greater than 0");

    if (m_cropLookup != NULL && m_cropNum > 0)
    {
        if (!m_cropLookupMap.empty())
            m_cropLookupMap.clear();
        for (int i = 0; i < m_cropNum; i++)
            m_cropLookupMap[(int) m_cropLookup[i][0]] = m_cropLookup[i];
    }
}

void MGTOpt_SWAT::initializeFertilizerLookup()
{
    /// Check input data
    if (m_fertilizerLookup == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Fertilizer lookup array must not be NULL");
    if (m_fertilizerNum <= 0)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Fertilizer number must be greater than 0");

    if (m_fertilizerLookup != NULL && m_fertilizerNum > 0)
    {
        if (!m_fertilizerLookupMap.empty())
            m_fertilizerLookupMap.clear();
        for (int i = 0; i < m_fertilizerNum; i++)
            m_fertilizerLookupMap[(int) m_fertilizerLookup[i][0]] = m_fertilizerLookup[i];
    }
}

void MGTOpt_SWAT::initializeTillageLookup()
{
    /// Check input data
    if (m_tillageLookup == NULL)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Tillage lookup array must not be NULL");
    if (m_tillageNum <= 0)
        throw ModelException(MID_MGT_SWAT, "CheckInputData", "Tillage number must be greater than 0");

    if (m_tillageLookup != NULL && m_tillageNum > 0)
    {
        if (!m_tillageLookupMap.empty())
            m_tillageLookupMap.clear();
        for (int i = 0; i < m_tillageNum; i++)
            m_tillageLookupMap[(int) m_tillageLookup[i][0]] = m_tillageLookup[i];
    }
}

void MGTOpt_SWAT::ExecutePlantOperation(int i, int &factoryID, int nOp)
{
    initializeLanduseLookup();
    PlantOperation *curOperation = (PlantOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    /// initialize parameters
    m_igro[i] = 1;
    m_HarvestIdxTarg[i] = curOperation->HITarg();
    m_BiomassTarg[i] = curOperation->BIOTarg(); /// kg/ha
    m_curYearMat[i] = curOperation->CurYearMaturity();
    m_landCover[i] = curOperation->PlantID();
    m_phuPlant[i] = curOperation->HeatUnits();
    m_dormFlag[i] = 0.f;
    m_phuAcc[i] = 0.f;
    m_plantN[i] = 0.f;
    m_plantP[i] = 0.f;
    m_pltET[i] = 0.f;
    m_pltPET[i] = 0.f;
    m_LAIMaxFr[i] = 0.f;
    m_havstIdxAdj[i] = 0.f;
    m_oLAI[i] = 0.f;
    m_frRoot[i] = 0.f;

    /// initialize transplant variables
    if (curOperation->LAIInit() > 0.f)
    {
        m_LAIDay[i] = curOperation->LAIInit();
        m_biomass[i] = curOperation->BIOInit();
    }

    /// compare maximum rooting depth in soil to maximum rooting depth of plant
    m_soilZMX[i] = m_soilDepth[i][(int) m_nSoilLayers[i] - 1];
    float pltRootDepth = m_landuseLookupMap[(int) m_landCover[i]][LANDUSE_PARAM_ROOT_DEPTH_IDX];
    m_soilZMX[i] = min(m_soilZMX[i], pltRootDepth);

    /// reset curve number if necessary
    if (curOperation->CNOP() > 0.f)   /// curno.f
    {
        float cnn = curOperation->CNOP();
        m_CN2[i] = cnn;
    }
}

void MGTOpt_SWAT::ExecuteIrrigationOperation(int i, int &factoryID, int nOp)
{
    IrrigationOperation *curOperation = (IrrigationOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    /// initialize parameters
    /// irrigation source
    int m_irrSource;
    /// irrigation source location code
    int m_irrNo;
    /// irrigation apply depth (mm)
    float m_irrApplyDepth;
    /// float* m_irrSalt; /// currently not used
    /// irrigation efficiency
    float m_irrEfficiency;
    m_irrSource = curOperation->IRRSource();
    m_irrNo = (curOperation->IRRNo() <= 0) ? (int) m_subBsnID[i] : curOperation->IRRNo();
    m_irrApplyDepth = curOperation->IRRApplyDepth();
    m_irrEfficiency = curOperation->IRREfficiency();
    m_irrFlag[i] = 1;
    if (m_irrSource > IRR_SRC_RES) /// irrigation from reach and reservoir are irr_rch.f and irr_res.f, respectively
    {
        /// call irrsub.f
        /// Performs the irrigation operation when the source is the shallow or deep aquifer or a source outside the watershed
        float vmma = 0.f; /// amount of water in source, mm
        float vmm = 0.f;  ///maximum amount of water to be applied, mm
        float cnv = 0.f;    /// conversion factor (mm/ha => m^3)
        float vmxi = 0.f;   /// amount of water specified in irrigation operation, mm
        float vol = 0.f;      /// volume of water to be applied in irrigation, m^3
        float vmms = 0.f; /// amount of water in shallow aquifer, m^3
        float vmmd = 0.f; /// amount of water in deep aquifer, m^3
        /// Whether m_irrNo is valid
        if (m_nCellsSubbsn.find(m_irrNo) == m_nCellsSubbsn.end())
            m_irrNo = int(m_subBsnID[i]);

        cnv = m_nAreaSubbsn[m_irrNo] * 10.f; /// area of current subbasin
        switch (m_irrSource)
        {
            /// in SEIMS, we hypothesis that shallow aquifer and deep aquifer is consistent within subbasin.
            case IRR_SRC_SHALLOW:
                if (m_shallowWaterDepth[i] < UTIL_ZERO)
                    m_shallowWaterDepth[i] = 0.f;
                vmma += m_shallowWaterDepth[i] * cnv * m_irrEfficiency;
                vmms = vmma;
                vmma /= m_nCellsSubbsn[m_irrNo];
                vmm = min(m_soilSumFC[i], vmma);
                break;
            case IRR_SRC_DEEP:
                vmma += m_deepWaterDepth[i] * cnv * m_irrEfficiency;
                vmmd = vmma;
                vmma /= m_nCellsSubbsn[m_irrNo];
                vmm = min(m_soilSumFC[i], vmma);
                break;
            case IRR_SRC_OUTWTSD: /// unlimited source
                vmm = m_soilSumFC[i];
                break;
        }
        /// if water available from source, proceed with irrigation
        if (vmm > 0.f)
        {
            cnv = m_cellArea * 10.f;
            vmxi = (m_irrApplyDepth < UTIL_ZERO) ? m_soilSumFC[i] : m_irrApplyDepth;
            if (vmm > vmxi) vmm = vmxi;
            vol = vmm * cnv;
            /// pot_fr(:)      |km2/km2       |fraction of cell area that drains into pothole
            float pot_fr = 0.001f; /// TODO
            /// m_impoundArea should be input from pothole.f
            /// currently, set a default value as half of cell's area. TODO
            m_impoundArea = m_cellArea / 10000.f * 0.5f;
            m_impoundVolume = 0.f;  /// TODO, should be DT_Raster1D
            if (pot_fr > UTIL_ZERO)
            {
                m_impoundVolume += vol / (10.f * m_impoundArea);
                m_appliedWater[i] = vmm;  ///added rice irrigation 11/10/11
            }
            else
            {
                /// Call irrigate(i, vmm) /// irrigate.f
                m_appliedWater[i] = vmm * (1.f - curOperation->IRRSQfrac());
                m_irrSurfQWater[i] = vmm * curOperation->IRRSQfrac();
            }
            /// subtract irrigation from shallow or deep aquifer
            if (pot_fr > UTIL_ZERO)
                vol = m_appliedWater[i] * cnv * m_irrEfficiency;
            switch (m_irrSource)
            {
                case IRR_SRC_SHALLOW:
                    cnv = m_nAreaSubbsn[m_irrNo] * 10.f;
                    vmma = 0.f;
                    if (vmms > -0.01f)
                        vmma = vol * m_shallowWaterDepth[i] * cnv / vmms;
                    vmma /= cnv;
                    m_shallowWaterDepth[i] -= vmma;
                    if (m_shallowWaterDepth[i] < 0.f)
                    {
                        vmma += m_shallowWaterDepth[i];
                        m_shallowWaterDepth[i] = 0.f;
                    }
                    m_shallowIrrWater[i] += vmma;
                    break;
                case IRR_SRC_DEEP:
                    cnv = m_nAreaSubbsn[m_irrNo] * 10.f;
                    vmma = 0.f;
                    if (vmmd > 0.01f)
                        vmma = vol * (m_deepWaterDepth[i] * cnv / vmmd);
                    vmma /= cnv;
                    m_deepWaterDepth[i] -= vmma;
                    if (m_deepWaterDepth[i] < 0.f)
                    {
                        vmma += m_deepWaterDepth[i];
                        m_deepWaterDepth[i] = 0.f;
                    }
                    m_deepIrrWater[i] += vmma;
                    break;
            }
        }
    }
}

void MGTOpt_SWAT::ExecuteFertilizerOperation(int i, int &factoryID, int nOp)
{
    /// translate from fert.f, remains CSWAT = 1 and 2 to be done!!! by LJ
    initializeFertilizerLookup();
    FertilizerOperation *curOperation = (FertilizerOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    /// initialize parameters
    int fertilizerID = curOperation->FertilizerID();
    /// kg/ha         |amount of fertilizer applied to HRU
    float fertilizerKg = curOperation->FertilizerKg();
    /// fraction of fertilizer which is applied to the top 10 mm of soil (the remaining
    /// fraction is applied to first soil layer)
    float fertilizerSurfFrac = curOperation->FertilizerSurfaceFrac();

    /**fertilizer paramters derived from lookup table**/
    //!!    fminn(:)      |kg minN/kg frt|fraction of fertilizer that is mineral N (NO3 + NH4)
    float fertMinN = m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_FMINN_IDX];
    //!!    fminp(:)      |kg minP/kg frt|fraction of fertilizer that is mineral P
    float fertMinP = m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_FMINP_IDX];
    //!!    forgn(:)      |kg orgN/kg frt|fraction of fertilizer that is organic N
    float fertOrgN = m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_FORGN_IDX];
    //!!    forgp(:)      |kg orgP/kg frt|fraction of fertilizer that is organic P
    float fertOrgP = m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_FORGP_IDX];
    //!!    fnh3n(:)      |kgNH3-N/kgminN|fraction of mineral N in fertilizer that is NH3-N
    float fertNH3N = m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_FNH3N_IDX];
    //!!    bactpdb(:)    |# cfu/g   frt |concentration of persistent bacteria in fertilizer
    float bactPDB = m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_BACTPDB_IDX];
    //!!    bactlpdb(:)   |# cfu/g   frt |concentration of less persistent bacteria in fertilizer
    float bactLPDB = m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_BATTLPDB_IDX];
    //!!    bactkddb(:)   |none          |fraction of bacteria in solution (the remaining fraction is sorbed to soil particles)
    float bactKDDB = m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_BACKTKDDB_IDX];

    /**summary output**/
    //!!    fertn         |kg N/ha       |total amount of nitrogen applied to soil in cell on day
    float fertN = 0.f;
    //!!    fertp         |kg P/ha       |total amount of phosphorus applied to soil in cell on day
    float fertP = 0.f;
    float fertNO3 = 0.f;
    float fertNH3 = 0.f;
    float fertSolP = 0.f;
    /// cfertn       |kg N/ha       |total amount of nitrogen applied to soil during continuous fertilizer operation in cell on day
    float cFertN = 0.f;
    /// cfertp       |kg P/ha       |total amount of phosphorus applied to soil during continuous fertilizer operation in cell on day
    float cFertP = 0.f;
    /// weighting factor used to partition the  organic N & P content of the fertilizer
    /// between the fresh organic and the active organic pools
    float rtof = 0.5f;
    float xx; /// fraction of fertilizer applied to layer
    float gc = 0.f, gc1 = 0.f, swf = 0.f, frt_t = 0.f;

    for (int l = 0; l < 2; l++) /// top surface and first layer
    {
        if (l == 0) xx = fertilizerSurfFrac;
        if (l == 1) xx = 1.f - fertilizerSurfFrac;
        m_soilNO3[i][l] += xx * fertilizerKg * (1.f - fertNH3N) * fertMinN;
        if (m_cswat == 0)
        {
            m_soilFreshOrgN[i][l] += rtof * xx * fertilizerKg * fertOrgN;
            m_soilActiveOrgN[i][l] += (1.f - rtof) * xx * fertilizerKg * fertOrgN;
            m_soilFreshOrgP[i][l] += rtof * xx * fertilizerKg * fertOrgP;
            m_soilActiveOrgN[i][l] += (1.f - rtof) * xx * fertilizerKg * fertOrgP;
        }
        else if (m_cswat == 1) /// C-FARM one carbon pool model /// TODO
        {
        }
        else if (m_cswat == 2) /// CENTURY model /// TODO
        {
            float X1 = 0.f, X8 = 0.f, X10 = 0.f, XXX = 0.f, YY = 0.f;
            float ZZ = 0.f, XZ = 0.f, YZ = 0.f, RLN = 0.f, orgc_f = 0.f;
        }
        m_soilNH3[i][l] += xx * fertilizerKg * fertNH3N * fertMinN;
        m_soilSolP[i][l] += xx * fertilizerKg * fertMinP;
    }
    /// add bacteria - #cfu/g * t(manure)/ha * 1.e6g/t * ha/10,000m^2 = 100.
    /// calculate ground cover
    gc = (1.99532f - Erfc(1.333f * m_LAIDay[i] - 2.f)) / 2.1f;
    if (gc < 0.f) gc = 0.f;
    gc1 = 1.f - gc;
    /// bact_swf    |none          |fraction of manure containing active colony forming units (cfu)
    //frt_t = m_bactSwf * fertilizerKg / 1000.;
    //m_bactPersistPlt[i] += gc * bactPDB * frt_t * 100.;
    //m_bactLessPersistPlt[i] += gc * bactLPDB * frt_t * 100.;

    //m_bactPersistSol[i] += gc1 * bactPDB * frt_t * 100.;
    //m_bactPersistSol[i] *= bactKDDB;

    //m_bactPersistParticle[i] += gc1 * bactPDB * frt_t * 100.;
    //m_bactPersistParticle[i] *= (1. - bactKDDB);

    //m_bactLessPersistSol[i] += gc1 * bactLPDB *frt_t * 100.;
    //m_bactLessPersistSol[i] *= bactKDDB;

    //m_bactLessPersistParticle[i] += gc1 * bactLPDB * frt_t * 100.;
    //m_bactLessPersistParticle[i] *= (1. - bactKDDB);

    /// summary calculations, currently not used for output. TODO in the future.
    fertNO3 = fertilizerKg * fertMinN * (1.f - fertNH3N);
    fertNH3 = fertilizerKg * (fertMinN * fertNH3N);
    fertOrgN = fertilizerKg * fertOrgN;
    fertOrgP = fertilizerKg * fertOrgP;
    fertSolP = fertilizerKg * fertSolP;
    fertN += (fertilizerKg + cFertN) * (fertMinN + fertOrgN);
    fertP += (fertilizerKg + cFertP) * (fertMinP + fertOrgP);

    //m_tillageFertN[i] += fertN;
    //m_tillageFertP[i] += fertP;
}

void MGTOpt_SWAT::ExecutePesticideOperation(int i, int &factoryID, int nOp)
{
    /// TODO
}

void MGTOpt_SWAT::ExecuteHarvestKillOperation(int i, int &factoryID, int nOp)
{
    //// TODO: Yield is not set as outputs yet. by LJ
    /// harvkillop.f
    HarvestKillOperation *curOperation = (HarvestKillOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    /// initialize parameters
    float cnop = curOperation->CNOP();
    float wur = 0.f, hiad1 = 0.f;
    /// Get some parameters of current crop / landcover
    float hvsti = m_cropLookupMap[(int) m_landCover[i]][CROP_PARAM_IDX_HVSTI];
    float wsyf = m_cropLookupMap[(int) m_landCover[i]][CROP_PARAM_IDX_WSYF];
    int idc = (int) m_cropLookupMap[(int) m_landCover[i]][CROP_PARAM_IDX_IDC];
    float bio_leaf = m_cropLookupMap[(int) m_landCover[i]][CROP_PARAM_IDX_BIO_LEAF];
    float cnyld = m_cropLookupMap[(int) m_landCover[i]][CROP_PARAM_IDX_CNYLD];
    float cpyld = m_cropLookupMap[(int) m_landCover[i]][CROP_PARAM_IDX_CPYLD];
    if (m_HarvestIdxTarg[i] > 0.f)
        hiad1 = m_HarvestIdxTarg[i];
    else
    {
        if (m_pltPET[i] < 10.f)
            wur = 100.f;
        else
            wur = 100.f * m_pltET[i] / m_pltPET[i];
        hiad1 = (m_havstIdxAdj[i] - wsyf) * (wur / (wur + exp(6.13f - 0.0883f * wur))) + wsyf;
        if (hiad1 > hvsti) hiad1 = hvsti;
    }
    /// check if yield is from above or below ground
    float yield = 0.f, resnew = 0.f, rtresnew = 0.f;

    /// stover fraction during harvestkill operation
    float hi_ovr = curOperation->HarvestIndexOverride();
    float xx = curOperation->StoverFracRemoved();
    if (xx < UTIL_ZERO)
        xx = hi_ovr;
    if (hi_ovr > UTIL_ZERO)
    {
        yield = m_biomass[i] * hi_ovr;
        resnew = m_biomass[i] - yield;
    }
    else
    {
        if (idc == CROP_IDC_TREES)
        {
            yield = m_biomass[i] * (1.f - bio_leaf);
            resnew = m_biomass[i] - yield;
        }
        else
        {
            if (hvsti > 1.001f)
            {
                yield = m_biomass[i] * (1.f - 1.f / (1.f + hiad1));
                resnew = m_biomass[i] / (1.f + hiad1);
                resnew *= (1.f - xx);
            }
            else
            {
                yield = (1.f - m_frRoot[i]) * m_biomass[i] * hiad1;
                resnew = (1.f - m_frRoot[i]) * (1.f - hiad1) * m_biomass[i];
                /// remove stover during harvestKill operation
                resnew *= (1.f - xx);
                rtresnew = m_frRoot[i] * m_biomass[i];
            }
        }
    }
    if (yield < 0.f) yield = 0.f;
    if (resnew < 0.f) resnew = 0.f;
    if (rtresnew < 0.f) rtresnew = 0.f;
    if (m_cswat == 2)
    {/// TODO
    }
    /// calculate nutrient removed with yield
    float yldpst = 0.f, yieldn = 0.f, yieldp = 0.f;
    yieldn = yield * cnyld;
    yieldp = yield * cpyld;
    yieldn = min(yieldn, 0.80f * m_plantN[i]);
    yieldp = min(yieldp, 0.80f * m_plantP[i]);

    /// call rootfr.f to distributes dead root mass through the soil profile
    /// i.e., derive fraction of roots in each layer
    float *rtfr = new float[(int) m_nSoilLayers[i]];
    rootFraction(i, rtfr);

    /// fraction of N, P in residue (ff1) or roots (ff2)
    float ff1 = (1.f - hiad1) / (1.f - hiad1 + m_frRoot[i]);
    float ff2 = 1.f - ff1;
    /// update residue, N, P on soil surface
    m_soilRsd[i][0] += resnew;
    m_soilFreshOrgN[i][0] += ff1 * (m_plantN[i] - yieldn);
    m_soilFreshOrgP[i][0] += ff1 * (m_plantP[i] - yieldp);
    m_soilRsd[i][0] = max(m_soilRsd[i][0], 0.f);
    m_soilFreshOrgN[i][0] = max(m_soilFreshOrgN[i][0], 0.f);
    m_soilFreshOrgP[i][0] = max(m_soilFreshOrgP[i][0], 0.f);

    /// allocate dead roots, N, P to soil layers
    for (int l = 0; l < m_nSoilLayers[i]; l++)
    {
        m_soilRsd[i][l] += rtfr[l] * rtresnew;
        m_soilFreshOrgN[i][l] += rtfr[l] * ff2 * (m_plantN[i] - yieldn);
        m_soilFreshOrgP[i][l] += rtfr[l] * ff2 * (m_plantP[i] - yieldp);
    }
    if (cnop > 0.f)
        m_CN2[i] = cnop; /// TODO: Is necessary to isolate the curno.f function for SUR_CN?
    /// reset variables
    m_igro[i] = 0.f;
    m_dormFlag[i] = 0.f;
    m_biomass[i] = 0.f;
    m_frRoot[i] = 0.f;
    m_plantN[i] = 0.f;
    m_plantP[i] = 0.f;
    m_frStrsWa[i] = 1.f;
    m_LAIDay[i] = 0.f;
    m_havstIdxAdj[i] = 0.f;
    delete[] rtfr;
    //m_phuBase[i] = 0.f;/// TODO, need to figure it out if this is CORRECT???
    m_phuAcc[i] = 0.f;
}

void MGTOpt_SWAT::rootFraction(int i, float *&root_fr)
{
    float cum_rd = 0.f, cum_d = 0.f, cum_rf = 0.f, x1 = 0.f, x2 = 0.f;
    if (m_lastSoilRootDepth[i] < UTIL_ZERO)
    {
        root_fr[0] = 1.f;
        return;
    }
    /// Normalized Root Density = 1.15*exp[-11.7*NRD] + 0.022, where NRD = normalized rooting depth
    /// Parameters of Normalized Root Density Function from Dwyer et al 19xx
    float a = 1.15f, b = 11.7f, c = 0.022f, d = 0.12029f; /// Integral of Normalized Root Distribution Function  from 0 to 1 (normalized depth) = 0.12029
    int k;/// used as layer identifier
    for (int l = 0; l < (int) m_nSoilLayers[i]; l++)
    {
        cum_d += m_soilThick[i][l];
        if (cum_d >= m_lastSoilRootDepth[i]) cum_rd = m_lastSoilRootDepth[i];
        if (cum_d < m_lastSoilRootDepth[i]) cum_rd = cum_d;
        x1 = (cum_rd - m_soilThick[i][l]) / m_lastSoilRootDepth[i];
        x2 = cum_rd / m_lastSoilRootDepth[i];
        float xx1 = -b * x1;
        if (xx1 > 20.f) xx1 = 20.f;
        float xx2 = -b * x2;
        if (xx2 > 20.f) xx2 = 20.f;
        root_fr[l] = (a / b * (exp(xx1) - exp(xx2)) + c * (x2 - x1)) / d;
        float xx = cum_rf;
        cum_rf += root_fr[l];
        if (cum_rf > 1.f)
        {
            root_fr[l] = 1.f - xx;
            cum_rf = 1.f;
        }
        k = l;
        if (cum_rd >= m_lastSoilRootDepth[i])
            break;
    }
    /// ensures that cumulative fractional root distribution = 1
    for (int l = 0; l < (int) m_nSoilLayers[i]; l++)
    {
        root_fr[l] /= cum_rf;
        if (l = k) /// exits loop on the same layer as the previous loop
            break;
    }
}

void MGTOpt_SWAT::ExecuteTillageOperation(int i, int &factoryID, int nOp)
{
    /// newtillmix.f
    /// Mix residue and nutrients during tillage and biological mixing
    initializeTillageLookup();
    TillageOperation *curOperation = (TillageOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    /// initialize parameters
    int tillID = curOperation->TillageID();
    float cnop = curOperation->CNOP();
    float deptil = m_tillageLookupMap[tillID][TILLAGE_PARAM_DEPTIL_IDX];
    float effmix = m_tillageLookupMap[tillID][TILLAGE_PARAM_EFFMIX_IDX];
    float bmix = 0.f;
    float emix = 0.f, dtil = 0.f, XX = 0.f, WW1 = 0.f, WW2 = 0.f;
    float WW3 = 0.f, WW4 = 0.f, maxmix = 0.f;
    float *soilMass = new float[(int) m_nSoilLayers[i]];
    float *soilMixedMass = new float[(int) m_nSoilLayers[i]];
    float *soilNotMixedMass = new float[(int) m_nSoilLayers[i]];

    if (bmix > UTIL_ZERO)
    {
        /// biological mixing, TODO to find usage elsewhere
        emix = bmix;
        dtil = min(m_soilDepth[i][(int) m_nSoilLayers[i] - 1], 50.f);
    }
    else
    {
        /// tillage operation
        emix = effmix;
        dtil = deptil;
    }
    if (tillID >= 1)
    {
        /// TODO, this is associated with erosion modules.
        ///Updated dynamic depressional storage D.Moriasi 4/8/2014
        //cumei(jj)   = 0.
        //cumeira(jj) = 0.
        //cumrt(jj)   = 0.
        //cumrai(jj)  = 0.
        //ranrns_hru(jj) = ranrns(idtill)
    }
    ///  smix(:)     |varies        |amount of substance in soil profile that is being redistributed between mixed layers
    int npmx = 0; /// number of different pesticides, TODO in next version
    float *smix = new float[22 + npmx + 12];
    for (int k = 0; k < 22 + npmx + 12; k++)
        smix[k] = 0.f;
    /// incorporate bacteria, no mixing, and bost from transport
    if (dtil > 10.f)
    {
        //m_bactPersistSol[i] *= (1. - emix);
        //m_bactPersistParticle[i] *= (1. - emix);
        //m_bactLessPersistSol[i] *= (1. - emix);
        //m_bactLessPersistParticle[i] *= (1. - emix);
    }
    /// calculate max mixing to preserve target surface residue
    /// Assume residue in all other layers is negligible to simplify calculation and remove depth dependency
    /// TODO, m_minResidue is defined in OPS which should be implemented later. By LJ
    float m_minResidue = 0.f;
    if (m_minResidue > 1.f && bmix < 0.001f)
    {
        maxmix = 1.f - m_minResidue / m_soilRsd[i][0];
        if (maxmix < 0.05f) maxmix = 0.05f;
        if (emix > maxmix) emix = maxmix;
    }
    for (int l = 0; l < (int) m_nSoilLayers[i]; l++)
    {
        soilMass[l] = (m_soilThick[i][l] / 1000.f) * 10000.f * m_soilBD[i][l] * 1000.f * (1 - m_soilRock[i][l] / 100.f);
        soilMixedMass[l] = 0.f;
        soilNotMixedMass[l] = 0.f;
    }
    if (dtil > 0.f)
    {
        if (dtil < 10.f) dtil = 11.;
        for (int l = 0; l < (int) m_nSoilLayers[i]; l++)
        {
            if (m_soilDepth[i][l] <= dtil)
            {
                soilMixedMass[l] = emix * soilMass[l];
                soilNotMixedMass[l] = soilMass[l] - soilMixedMass[l];
            }
            else if (m_soilDepth[i][l] > dtil && m_soilDepth[i][l - 1] < dtil)
            {
                soilMixedMass[l] = emix * soilMass[l] * (dtil - m_soilDepth[i][l - 1]) / m_soilThick[i][l];
                soilNotMixedMass[l] = soilMass[l] - soilMixedMass[l];
            }
            else
            {
                soilMixedMass[l] = 0.f;
                soilNotMixedMass[l] = soilMass[l];
            }
            /// calculate the mass or concentration of each mixed element
            /// 1. mass based mixing
            WW1 = soilMixedMass[l] / (soilMixedMass[l] + soilNotMixedMass[l]);
            smix[0] += m_soilNO3[i][l] * WW1;
            smix[1] += m_soilStableOrgN[i][l] * WW1;
            smix[2] += m_soilNH3[i][l] * WW1;
            smix[3] += m_soilSolP[i][l] * WW1;
            smix[4] += m_soilOrgP[i][l] * WW1;
            smix[5] += m_soilActiveOrgN[i][l] * WW1;
            smix[6] += m_soilActiveMinP[i][l] * WW1;
            smix[7] += m_soilFreshOrgN[i][l] * WW1;
            smix[8] += m_soilFreshOrgP[i][l] * WW1;
            smix[9] += m_soilStableMinP[i][l] * WW1;
            smix[10] += m_soilRsd[i][l] * WW1;
            //smix[11] += m_soilMC[i][l] * WW1; /// TODO when cswat == 1 is done. by LJ
            //smix[12] += m_soilMN[i][l] * WW1;
            //smix[13] += m_soilMP[i][l] * WW1;

            /// 2. concentration based mixing
            WW2 = XX + soilMixedMass[l];
            smix[14] = (XX * smix[14] + m_soilCarbon[i][l] * soilMixedMass[l]) / WW2;
            smix[15] = (XX * smix[15] + m_soilN[i][l] * soilMixedMass[l]) / WW2;
            smix[16] = (XX * smix[16] + m_soilClay[i][l] * soilMixedMass[l]) / WW2;
            smix[17] = (XX * smix[17] + m_soilSilt[i][l] * soilMixedMass[l]) / WW2;
            smix[18] = (XX * smix[18] + m_soilSand[i][l] * soilMixedMass[l]) / WW2;
            /// 3. mass based distribution
            for (int k = 0; k < npmx; k++)
            {
                /// TODO
            }
            XX += soilMixedMass[l];
        }
        for (int l = 0; l < (int) m_nSoilLayers[i]; l++)
        {
            /// reconstitute each soil layer
            WW3 = soilNotMixedMass[l] / soilMass[l];
            WW4 = soilMixedMass[l] / XX;
            m_soilNO3[i][l] = m_soilNO3[i][l] * WW3 + smix[0] * WW4;
            m_soilStableOrgN[i][l] = m_soilStableOrgN[i][l] * WW3 + smix[1] * WW4;
            m_soilNH3[i][l] = m_soilNH3[i][l] * WW3 + smix[2] * WW4;
            m_soilSolP[i][l] = m_soilSolP[i][l] * WW3 + smix[3] * WW4;
            m_soilOrgP[i][l] = m_soilOrgP[i][l] * WW3 + smix[4] * WW4;
            m_soilActiveOrgN[i][l] = m_soilActiveOrgN[i][l] * WW3 + smix[5] * WW4;
            m_soilActiveMinP[i][l] = m_soilActiveMinP[i][l] * WW3 + smix[6] * WW4;
            m_soilFreshOrgN[i][l] = m_soilFreshOrgN[i][l] * WW3 + smix[7] * WW4;
            m_soilFreshOrgP[i][l] = m_soilFreshOrgP[i][l] * WW3 + smix[8] * WW4;
            m_soilStableMinP[i][l] = m_soilStableMinP[i][l] * WW3 + smix[9] * WW4;
            m_soilRsd[i][l] = m_soilRsd[i][l] * WW3 + smix[10] * WW4;
            if (m_soilRsd[i][l] < 1.e-10f) m_soilRsd[i][l] = 1.e-10f;

            m_soilCarbon[i][l] = (m_soilCarbon[i][l] * soilNotMixedMass[l] + smix[14] * soilMixedMass[l]) / soilMass[l];
            m_soilN[i][l] = (m_soilN[i][l] * soilNotMixedMass[l] + smix[15] * soilMixedMass[l]) / soilMass[l];
            m_soilClay[i][l] = (m_soilClay[i][l] * soilNotMixedMass[l] + smix[16] * soilMixedMass[l]) / soilMass[l];
            m_soilSilt[i][l] = (m_soilSilt[i][l] * soilNotMixedMass[l] + smix[17] * soilMixedMass[l]) / soilMass[l];
            m_soilSand[i][l] = (m_soilSand[i][l] * soilNotMixedMass[l] + smix[18] * soilMixedMass[l]) / soilMass[l];
            if (m_cswat == 1)
            {
                /// TODO
            }
            if (m_cswat == 2)
            {
                /// TODO
            }
        }
    }
    if (cnop > 1.e-4f) m_CN2[i] = cnop;
    delete[] soilMass;
    delete[] soilMixedMass;
    delete[] soilNotMixedMass;
}

void MGTOpt_SWAT::ExecuteHarvestOnlyOperation(int i, int &factoryID, int nOp)
{
    /// harvestop.f
    ///
    HarvestOnlyOperation *curOperation = (HarvestOnlyOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    /// initialize parameters
    float hi_bms = curOperation->HarvestIndexBiomass();
    float hi_rsd = curOperation->HarvestIndexResidue();
    float harveff = curOperation->HarvestEfficiency();
    /// Get some parameters of current crop / landcover
    float hvsti = m_cropLookupMap[(int) m_landCover[i]][CROP_PARAM_IDX_HVSTI];
    float wsyf = m_cropLookupMap[(int) m_landCover[i]][CROP_PARAM_IDX_WSYF];
    int idc = (int) m_cropLookupMap[(int) m_landCover[i]][CROP_PARAM_IDX_IDC];
    float bio_leaf = m_cropLookupMap[(int) m_landCover[i]][CROP_PARAM_IDX_BIO_LEAF];
    float cnyld = m_cropLookupMap[(int) m_landCover[i]][CROP_PARAM_IDX_CNYLD];
    float cpyld = m_cropLookupMap[(int) m_landCover[i]][CROP_PARAM_IDX_CPYLD];
    float alai_min = m_cropLookupMap[(int) m_landCover[i]][CROP_PARAM_IDX_ALAI_MIN];

    float yieldTbr = 0.f, yieldNtbr = 0.f, yieldPtbr = 0.f;
    float clipTbr = 0.f, clipNtbr = 0.f, clipPtbr = 0.f;
    float ssb = m_biomass[i];
    float ssabg = m_biomass[i] * (1.f - m_frRoot[i]);
    float ssr = ssb * m_frRoot[i];
    float ssn = m_plantN[i];
    float ssp = m_plantP[i];
    /// calculate modifier for auto fertilization target nitrogen content
    m_targNYld[i] = (1.f - m_frRoot[i] * m_biomass[i] * m_frPlantN[i] * m_autoFertEfficiency[i]);
    /// compute grain yield
    float hiad1 = 0.f; /// hiad1       |none           |actual harvest index (adj for water/growth)
    float wur = 0.f; /// wur         |none           |water deficiency factor
    if (m_pltPET[i] < 10.f)
        wur = 100.f;
    else
        wur = 100.f * m_pltET[i] / m_pltPET[i];
    hiad1 = (m_havstIdxAdj[i] - wsyf) * (wur / (wur + exp(6.13f - 0.0883f * wur))) + wsyf;
    if (hiad1 > hvsti) hiad1 = hvsti;
    /// check if yield is from above or below ground
    if (hvsti > 1.001f)
    {
        /// determine clippings (biomass left behind) and update yield
        yieldTbr = m_biomass[i] * (1.f - 1.f / (1.f + hiad1)) * harveff;
        clipTbr = m_biomass[i] * (1.f - 1.f / (1.f + hiad1)) * (1.f - harveff);
        m_biomass[i] -= (yieldTbr + clipTbr);
        /// calculate nutrients removed with yield
        yieldNtbr = min(yieldTbr * cnyld, 0.80f * m_plantN[i]);
        yieldPtbr = min(yieldTbr * cpyld, 0.80f * m_plantP[i]);
        /// calculate nutrients removed with clippings
        clipNtbr = min(clipTbr * m_frPlantN[i], m_plantN[i] - yieldNtbr);
        clipPtbr = min(clipTbr * m_frPlantP[i], m_plantP[i] - yieldPtbr);
        m_plantN[i] -= (yieldNtbr + clipNtbr);
        m_plantP[i] -= (yieldPtbr + clipPtbr);
    }
    float yieldBms = 0.f, yieldNbms = 0.f, yieldPbms = 0.f;
    float clipBms = 0.f, clipNbms = 0.f, clipPbms = 0.f;
    float yieldGrn = 0.f, yieldNgrn = 0.f, yieldPgrn = 0.f;
    float clipGrn = 0.f, clipNgrn = 0.f, clipPgrn = 0.f;

    if (hi_bms > 0.f)
    {
        /// compute biomass yield
        yieldBms = hi_bms * (1.f - m_frRoot[i]) * m_biomass[i] * harveff;
        clipBms = hi_bms * (1.f - m_frRoot[i]) * m_biomass[i] * (1.f - harveff);
        m_biomass[i] -= (yieldBms + clipBms);
        /// compute nutrients removed with yield
        yieldNbms = min(yieldBms * cnyld, 0.80f * m_plantN[i]);
        yieldPbms = min(yieldBms * cpyld, 0.80f * m_plantP[i]);
        /// calculate nutrients removed with clippings
        clipNbms = min(clipBms * m_frPlantN[i], m_plantN[i] - yieldNbms);
        clipPbms = min(clipBms * m_frPlantP[i], m_plantP[i] - yieldPbms);
        m_plantN[i] -= (yieldNbms + clipNbms);
        m_plantP[i] -= (yieldPbms + clipPbms);
    }
    else
    {
        /// compute grain yields
        yieldGrn = (1.f - m_frRoot[i]) * m_biomass[i] * hiad1 * harveff;
        /// determine clippings (biomass left behind) and update yield
        clipGrn = (1.f - m_frRoot[i]) * m_biomass[i] * hiad1 * (1.f - harveff);
        m_biomass[i] -= (yieldGrn + clipGrn);
        /// calculate nutrients removed with yield
        yieldNgrn = min(yieldGrn * cnyld, 0.80f * m_plantN[i]);
        yieldPgrn = min(yieldGrn * cpyld, 0.80f * m_plantP[i]);
        /// calculate nutrients removed with clippings
        clipNgrn = min(clipGrn * m_frPlantN[i], m_plantN[i] - yieldNgrn);
        clipPgrn = min(clipGrn * m_frPlantP[i], m_plantP[i] - yieldPgrn);
        m_plantN[i] -= (yieldNgrn + clipNgrn);
        m_plantP[i] -= (yieldPgrn + clipPgrn);
    }
    /// add clippings to residue and organic N and P
    m_soilRsd[i][0] += (clipGrn + clipBms + clipTbr);
    m_soilFreshOrgN[i][0] += (clipNgrn + clipNbms + clipNtbr);
    m_soilFreshOrgP[i][0] += (clipPgrn + clipPbms + clipPtbr);
    /// compute residue yield
    float yieldRsd = 0.f, yieldNrsd = 0.f, yieldPrsd = 0.f;
    if (hi_rsd > 0.)
    {
        yieldRsd = hi_rsd * m_soilRsd[i][0];
        yieldNrsd = hi_rsd * m_soilFreshOrgN[i][0];
        yieldPrsd = hi_rsd * m_soilFreshOrgP[i][0];
        m_soilRsd[i][0] -= yieldRsd;
        m_soilFreshOrgN[i][0] -= yieldNrsd;
        m_soilFreshOrgP[i][0] -= yieldPrsd;
    }
    float yield = 0.f, yieldN = 0.f, yieldP = 0.f;
    yield = yieldGrn + yieldBms + yieldTbr + yieldRsd;
    yieldN = yieldNgrn + yieldNbms + yieldNtbr + yieldNrsd;
    yieldP = yieldPgrn + yieldPbms + yieldPtbr + yieldPrsd;
    float clip = 0.f, clipN = 0.f, clipP = 0.f;
    clip = clipGrn + clipBms + clipTbr;
    clipN = clipNgrn + clipNbms + clipNtbr;
    clipP = clipPgrn + clipPbms + clipNtbr;

    /// Calculation for dead roots allocations, resetting phenology, updating other pools
    float ff3 = 0.f;
    if (ssabg > UTIL_ZERO)
        ff3 = (yield + clip) / ssabg;
    else
        ff3 = 1.f;
    if (ff3 > 1.f) ff3 = 1.f;
    /// reset leaf area index and fraction of growing season
    if (ssb > 0.001f)
    {
        m_LAIDay[i] *= (1.f - ff3);
        if (m_LAIDay[i] < alai_min)
            m_LAIDay[i] = alai_min;
        m_phuAcc[i] *= (1.f - ff3);
        m_frRoot[i] = 0.4f - 0.2f * m_phuAcc[i];
    }
    else
    {
        m_biomass[i] = 0.f;
        m_LAIDay[i] = 0.f;
        m_phuAcc[i] = 0.f;
    }
    /// allocate roots, N, and P to soil pools
    for (int l = 0; l < (int) m_nSoilLayers[i]; l++)
    {
        /// TODO, check it out in the near future.
    }
}

void MGTOpt_SWAT::ExecuteKillOperation(int i, int &factoryID, int nOp)
{
    /// killop.f
    KillOperation *curOperation = (KillOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    float resnew = 0.f, rtresnew = 0.f;
    resnew = m_biomass[i] * (1.f - m_frRoot[i]);
    rtresnew = m_biomass[i] * m_frRoot[i];
    /// call rootfr.f to distributes dead root mass through the soil profile
    /// i.e., derive fraction of roots in each layer
    float *rtfr = new float[(int) m_nSoilLayers[i]];
    rootFraction(i, rtfr);
    /// update residue, N, P on soil surface
    m_soilRsd[i][0] += resnew;
    m_soilFreshOrgN[i][0] += m_plantN[i] * (1.f - m_frRoot[i]);
    m_soilFreshOrgP[i][0] += m_plantP[i] * (1.f - m_frRoot[i]);
    m_soilRsd[i][0] = max(m_soilRsd[i][0], 0.f);
    m_soilFreshOrgN[i][0] = max(m_soilFreshOrgN[i][0], 0.f);
    m_soilFreshOrgP[i][0] = max(m_soilFreshOrgP[i][0], 0.f);

    /// allocate dead roots, N and P to soil layers
    for (int l = 0; l < (int) m_nSoilLayers[i]; l++)
    {
        m_soilRsd[i][l] += rtfr[l] * rtresnew;
        m_soilFreshOrgN[i][l] += rtfr[l] * m_plantN[i] * m_frRoot[i];
        m_soilFreshOrgP[i][l] += rtfr[l] * m_plantP[i] * m_frRoot[i];
    }
    /// reset variables
    m_igro[i] = 0;
    m_dormFlag[i] = 0;
    m_biomass[i] = 0.;
    m_frRoot[i] = 0.;
    m_plantN[i] = 0.;
    m_plantP[i] = 0.;
    m_frStrsWa[i] = 1.;
    m_LAIDay[i] = 0.;
    m_havstIdxAdj[i] = 0.;
    delete[] rtfr;
    //m_phuBase[i] = 0.; /// TODO, need to figure it out if this is CORRECT???
    m_phuAcc[i] = 0.;
}

void MGTOpt_SWAT::ExecuteGrazingOperation(int i, int &factoryID, int nOp)
{
    /// TODO
    /// graze.f, simulate biomass lost to grazing
    GrazingOperation *curOperation = (GrazingOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    int manureID = curOperation->ManureID();
    int grzDays = curOperation->GrazingDays();
    float bioEat = curOperation->BiomassConsumed();
    float bioTrmp = curOperation->BiomassTrampled();
    float manueKg = curOperation->ManureDeposited();
}

void MGTOpt_SWAT::ExecuteAutoIrrigationOperation(int i, int &factoryID, int nOp)
{
    AutoIrrigationOperation *curOperation = (AutoIrrigationOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    m_autoIrrSource[i] = curOperation->AutoIrrSrcCode();
    m_autoIrrNo[i] = (curOperation->AutoIrrSrcLocs() <= 0) ? (int) m_subBsnID[i] : curOperation->AutoIrrSrcLocs();
    m_wtrStrsID[i] = curOperation->WaterStrsIdent();
    m_autoWtrStres[i] = curOperation->AutoWtrStrsThrsd();
    m_autoIrrEfficiency[i] = curOperation->IrrigationEfficiency();
    m_autoIrrWtrDepth[i] = curOperation->IrrigationWaterApplied();
    m_autoSurfRunRatio[i] = curOperation->SurfaceRunoffRatio();
    m_irrFlag[i] = 1;
    /// call autoirr.f
    /// TODO, this will be implemented as an isolated module in the near future.
}

void MGTOpt_SWAT::ExecuteAutoFertilizerOperation(int i, int &factoryID, int nOp)
{
    AutoFertilizerOperation *curOperation = (AutoFertilizerOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    m_fertilizerID[i] = curOperation->FertilizerID();
    m_NStressCode[i] = curOperation->NitrogenMethod();
    m_autoNStress[i] = curOperation->NitrogenStrsFactor();
    m_autoMaxAppliedN[i] = curOperation->MaxMineralN();
    m_autoAnnMaxAppliedMinN[i] = curOperation->MaxMineralNYearly();
    m_autoFertEfficiency[i] = curOperation->FertEfficiency();
    m_autoFertSurface[i] = curOperation->SurfaceFracApplied();
    float cnyld = m_cropLookupMap[(int) m_landCover[i]][CROP_PARAM_IDX_CNYLD];
    float bio_e = m_cropLookupMap[(int) m_landCover[i]][CROP_PARAM_IDX_BIO_E];
    /// calculate modifier for auto fertilization target nitrogen content'
    if (m_targNYld[i] < UTIL_ZERO)
        m_targNYld[i] = 150.f * cnyld * bio_e;
    /// call anfert.f
    /// TODO, this will be implemented as an isolated module in the near future.
}

void MGTOpt_SWAT::ExecuteReleaseImpoundOperation(int i, int &factoryID, int nOp)
{
    /// No more executable code here.
    ReleaseImpoundOperation *curOperation = (ReleaseImpoundOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    m_impoundTriger[i] = curOperation->ImpoundTriger();
    /// pothole.f and potholehr.f for sub-daily timestep simulation
}

void MGTOpt_SWAT::ExecuteContinuousFertilizerOperation(int i, int &factoryID, int nOp)
{
    // TODO
    ContinuousFertilizerOperation *curOperation = (ContinuousFertilizerOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
}

void MGTOpt_SWAT::ExecuteContinuousPesticideOperation(int i, int &factoryID, int nOp)
{
    /// TODO
    ContinuousPesticideOperation *curOperation = (ContinuousPesticideOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];

}

void MGTOpt_SWAT::ExecuteBurningOperation(int i, int &factoryID, int nOp)
{
    BurningOperation *curOperation = (BurningOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    /// TODO
}

void MGTOpt_SWAT::ScheduledManagement(int cellIdx, int &factoryID, int nOp)
{
    /// nOp is index * 1000 + operationCode
    int mgtCode = nOp % 1000;
    switch (mgtCode)
    {
        case BMP_PLTOP_Plant:
            ExecutePlantOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_Irrigation:
            ExecuteIrrigationOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_Fertilizer:
            ExecuteFertilizerOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_Pesticide:
            ExecutePesticideOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_HarvestKill:
            ExecuteHarvestKillOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_Tillage:
            ExecuteTillageOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_Harvest:
            ExecuteHarvestOnlyOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_Kill:
            ExecuteKillOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_Grazing:
            ExecuteGrazingOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_AutoIrrigation:
            ExecuteAutoIrrigationOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_AutoFertilizer:
            ExecuteAutoFertilizerOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_ReleaseImpound:
            ExecuteReleaseImpoundOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_ContinuousFertilizer:
            ExecuteContinuousFertilizerOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_ContinuousPesticide:
            ExecuteContinuousPesticideOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_Burning:
            ExecuteBurningOperation(cellIdx, factoryID, nOp);
            break;
            //case BMP_PLTOP_SKIPYEAR:
            //	m_yearSkip[i] = 1;
            //	break;
    }
}

int MGTOpt_SWAT::Execute()
{
    CheckInputData();  /// essential input data, other inputs for specific management operation will be check separately.
    initialOutputs(); /// all possible outputs will be initialized to avoid NULL pointer problems.
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++)
    {
        int curFactoryID;
        vector<int> curOps;
        if (GetOperationCode(i, curFactoryID, curOps))
        {
            for (vector<int>::iterator it = curOps.begin(); it != curOps.end(); it++)
                ScheduledManagement(i, curFactoryID, *it);
        }
    }
    return true;
}

void MGTOpt_SWAT::Get1DData(const char *key, int *n, float **data)
{
    string sk(key);
    if (StringMatch(sk, VAR_IGRO)) *data = m_igro;
        /// plant operation
    else if (StringMatch(sk, VAR_HITARG)) *data = m_HarvestIdxTarg;
    else if (StringMatch(sk, VAR_BIOTARG)) *data = m_BiomassTarg;
        /// auto irrigation operation
    else if (StringMatch(sk, VAR_IRR_FLAG)) *data = m_irrFlag;
    else if (StringMatch(sk, VAR_IRR_WTR)) *data = m_appliedWater;
    else if (StringMatch(sk, VAR_IRR_SURFQ)) *data = m_irrSurfQWater;
    else if (StringMatch(sk, VAR_AWTR_STRS_ID)) *data = m_wtrStrsID;
    else if (StringMatch(sk, VAR_AWTR_STRS_TRIG)) *data = m_autoWtrStres;
    else if (StringMatch(sk, VAR_AIRR_SOURCE)) *data = m_autoIrrSource;
    else if (StringMatch(sk, VAR_AIRR_LOCATION)) *data = m_autoIrrNo;
    else if (StringMatch(sk, VAR_AIRR_EFF)) *data = m_autoIrrEfficiency;
    else if (StringMatch(sk, VAR_AIRRWTR_DEPTH)) *data = m_autoIrrWtrDepth;
    else if (StringMatch(sk, VAR_AIRRSURF_RATIO)) *data = m_autoSurfRunRatio;
        /// fertilizer / auto fertilizer operation
    else if (StringMatch(sk, VAR_AFERT_ID)) *data = m_fertilizerID;
    else if (StringMatch(sk, VAR_AFERT_NSTRSID)) *data = m_NStressCode;
    else if (StringMatch(sk, VAR_AFERT_NSTRS)) *data = m_autoNStress;
    else if (StringMatch(sk, VAR_AFERT_MAXN)) *data = m_autoMaxAppliedN;
    else if (StringMatch(sk, VAR_AFERT_AMAXN)) *data = m_autoAnnMaxAppliedMinN;
    else if (StringMatch(sk, VAR_AFERT_NYLDT)) *data = m_targNYld;
    else if (StringMatch(sk, VAR_AFERT_FRTEFF)) *data = m_autoFertEfficiency;
    else if (StringMatch(sk, VAR_AFERT_FRTSURF)) *data = m_autoFertSurface;
        /// Grazing operation
    else if (StringMatch(sk, VAR_GRZ_DAYS)) *data = m_nGrazingDays;
    else if (StringMatch(sk, VAR_GRZ_FLAG)) *data = m_grzFlag;
        /// Impound/Release operation
    else if (StringMatch(sk, VAR_IMPOUND_TRIG)) *data = m_impoundTriger;
    *n = m_nCells;
}

void MGTOpt_SWAT::Get2DData(const char *key, int *n, int *col, float ***data)
{
    string sk(key);
    /// There are no Raster2D data to output. by LJ
}

void MGTOpt_SWAT::initialOutputs()
{
    m_cellArea = m_cellWidth * m_cellWidth / 10000.f;
    if (m_nSub <= 0)
    {
#pragma omp parallel for
        for (int i = 0; i < this->m_nCells; i++)
        {
            m_nCellsSubbsn[int(this->m_subBsnID[i])] += 1;
            m_nAreaSubbsn[int(this->m_subBsnID[i])] += m_cellArea;
        }
        this->m_nSub = m_nCellsSubbsn.size();
    }
    /// plant operation
    if (m_HarvestIdxTarg == NULL) Initialize1DArray(m_nCells, m_HarvestIdxTarg, 0.f);
    if (m_BiomassTarg == NULL) Initialize1DArray(m_nCells, m_BiomassTarg, 0.f);
    /// irrigation / auto irrigation operations
    if (m_appliedWater == NULL) Initialize1DArray(m_nCells, m_appliedWater, 0.f);
    if (m_irrSurfQWater = NULL) Initialize1DArray(m_nCells, m_irrSurfQWater, 0.f);
    if (m_irrFlag == NULL) Initialize1DArray(m_nCells, m_irrFlag, 0.f);
    if (m_autoIrrSource == NULL) Initialize1DArray(m_nCells, m_autoIrrSource, (float) IRR_SRC_OUTWTSD);
    if (m_autoIrrNo == NULL) Initialize1DArray(m_nCells, m_autoIrrNo, -1.f);
    if (m_wtrStrsID == NULL) Initialize1DArray(m_nCells, m_wtrStrsID, 1.f); /// By default, plant water demand
    if (m_autoWtrStres == NULL) Initialize1DArray(m_nCells, m_autoWtrStres, 0.f);
    if (m_autoIrrEfficiency == NULL) Initialize1DArray(m_nCells, m_autoIrrEfficiency, 0.f);
    if (m_autoIrrWtrDepth == NULL) Initialize1DArray(m_nCells, m_autoIrrWtrDepth, 0.f);
    if (m_autoSurfRunRatio == NULL) Initialize1DArray(m_nCells, m_autoSurfRunRatio, 0.f);
    /// fertilizer / auto fertilizer operations
    if (m_fertilizerID == NULL) Initialize1DArray(m_nCells, m_fertilizerID, -1.f);
    if (m_NStressCode == NULL) Initialize1DArray(m_nCells, m_NStressCode, 0.f);
    if (m_autoNStress == NULL) Initialize1DArray(m_nCells, m_autoNStress, 0.f);
    if (m_autoMaxAppliedN == NULL) Initialize1DArray(m_nCells, m_autoMaxAppliedN, 0.f);
    if (m_targNYld == NULL) Initialize1DArray(m_nCells, m_targNYld, 0.f);
    if (m_autoAnnMaxAppliedMinN == NULL) Initialize1DArray(m_nCells, m_autoAnnMaxAppliedMinN, 0.f);
    if (m_autoFertEfficiency == NULL) Initialize1DArray(m_nCells, m_autoFertEfficiency, 0.f);
    if (m_autoFertSurface == NULL) Initialize1DArray(m_nCells, m_autoFertSurface, 0.f);
}

float MGTOpt_SWAT::Erfc(float xx)
{
    float c1 = .19684f, c2 = .115194f;
    float c3 = .00034f, c4 = .019527f;
    float x = 0.f, erf = 0.f, erfc = 0.f;
    x = abs(sqrt(2.f) * xx);
    erf = 1.f - pow(float(1.f + c1 * x + c2 * x * x + c3 * pow(x, 3.f) + c4 * pow(x, 4.f)), -4.f);
    if (xx < 0.f) erf = -erf;
    erfc = 1.f - erf;
    return erfc;
}