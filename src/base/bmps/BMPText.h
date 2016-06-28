/*!
 * \file BMPText.h
 * \brief BMP related const strings
 * \revised Liang-Jun Zhu
 * \date 2016-6-16
 */
#ifndef BMP_TEXT_H
#define BMP_TEXT_H

//database name
#define BMP_DATABASE_NAME	"BMP.db3"

//Table names in BMP database
#define TAB_BMP_INDEX				"BMP_index"
#define TAB_BMP_SCENARIO		"BMP_scenarios"
#define TAB_BMP_REACH				"Reach_BMP"
#define TAB_BMP_POINT_SRC	"pointsource"
#define TAB_BMP_FLOWDIVERSION	"flowdiversion"
#define TAB_BMP_RESERVOIR	"reservoir"

/// Table field names 
/// TAB_BMP_SCENARIO
#define FLD_SCENARIO_ID				"ID"
#define FLD_SCENARIO_NAME		"NAME"
#define FLD_SCENARIO_BMPID		"BMPID"
#define FLD_SCENARIO_SUB			"SUBSCENARIO"
#define FLD_SCENARIO_DIST			"DISTRIBUTION"
#define FLD_SCENARIO_LOOKUP	"COLLECTION"
#define FLD_SCENARIO_LOCATION "LOCATION"
/// TAB_BMP_INDEX
#define FLD_BMP_ID							"ID"
#define FLD_BMP_TYPE						"TYPE"
#define FLD_BMP_PRIORITY			"PRIORITY"

//BMPs
//also the column name in Reach_BMP table for reach BMPs
#define BMP_NAME_POINTSOURCE			"PointSource"
#define BMP_NAME_FLOWDIVERSION_STREAM	"FlowDiversion"
#define BMP_NAME_RESERVOIR				"Reservoir"
#define BMP_NAME_RIPARIANWETLAND		"RiparianWetland"
#define BMP_NAME_RIPARIANBUFFER			"RiparianBuffer"
#define BMP_NAME_GRASSWATERWAY			"Grass Water Way"
#define BMP_NAME_FILTERSTRIP			"Filter Strip"
#define BMP_NAME_POND					"Pond"
#define BMP_NAME_ISOLATEDPOND			"Isolated Pond"
#define BMP_NAME_TERRACE				"Terrace"
#define BMP_NAME_FLOWDIVERSION_OVERLAND	"Overland Flow Diversion"
#define BMP_NAME_CROP					"Crop Management"
#define BMP_NAME_RESIDUAL				"Residual Management"
#define BMP_NAME_TILLAGE				"Tillage Management"
#define BMP_NAME_FERTILIZER				"Fertilizer Management"
#define BMP_NAME_GRAZING				"Grazing Management"
#define BMP_NAME_PESTICIDE				"Pesticide Management"
#define BMP_NAME_IRRIGATION				"Irrigation Management"
#define BMP_NAME_TILEDRAIN				"Tile Drain Management"
#define BMP_NAME_URBAN					"Urban Management"

//the hydro climate data columns of pointsource, flowdiversion and reservoir
#define BMP_REACH_FLOW_NAME			"flow"
#define BMP_REACH_SEDIMENT_NAME		"sediment"
#define BMP_REACH_ORGANICN_NAME		"organicN"
#define BMP_REACH_ORGANICP_NAME		"organicP"
#define BMP_REACH_NO3_NAME			"NO3"
#define BMP_REACH_NH3_NAME			"NH3"
#define BMP_REACH_NO2_NAME			"NO2"
#define BMP_REACH_MINERALP_NAME		"mineralP"

#define BMP_REACH_UNKONWN_TYPE		-1
#define BMP_REACH_FLOW_TYPE			1
#define BMP_REACH_SEDIMENT_TYPE		2
#define BMP_REACH_ORGANICN_TYPE		3
#define BMP_REACH_ORGANICP_TYPE		4
#define BMP_REACH_NO3_TYPE			5
#define BMP_REACH_NH3_TYPE			6
#define BMP_REACH_NO2_TYPE			7
#define BMP_REACH_MINERALP_TYPE		8

#define BMP_REACH_X_INDEX				1
#define BMP_REACH_Y_INDEX				2



//base scenario id
#define BASE_SCENARIO_ID	0

//BMP Type
#define BMP_TYPE_POINTSOURCE			1
#define BMP_TYPE_FLOWDIVERSION_STREAM	2
#define BMP_TYPE_RESERVOIR				3
#define BMP_TYPE_RIPARIANWETLAND		4
#define BMP_TYPE_RIPARIANBUFFER			5
#define BMP_TYPE_GRASSWATERWAY			6
#define BMP_TYPE_FILTERSTRIP			7
#define BMP_TYPE_POND					8
#define BMP_TYPE_ISOLATEDPOND			9
#define BMP_TYPE_TERRACE				10
#define BMP_TYPE_FLOWDIVERSION_OVERLAND	11
#define BMP_TYPE_PLANT_MGT		12
/// TODO
#define BMP_TYPE_CROP					12
#define BMP_TYPE_RESIDUAL				13
#define BMP_TYPE_TILLAGE				14
#define BMP_TYPE_FERTILIZER				15
#define BMP_TYPE_GRAZING				16
#define BMP_TYPE_PESTICIDE				17
#define BMP_TYPE_IRRIGATION				18
#define BMP_TYPE_TILEDRAIN				19
#define BMP_TYPE_URBAN					20

/// Plant management code
#define BMP_PLTOP_Plant					1
#define BMP_PLTOP_Irrigation			2
#define BMP_PLTOP_Fertilizer			3
#define BMP_PLTOP_Pesticide			4
#define BMP_PLTOP_HarvestKill		5
#define BMP_PLTOP_Tillage				6
#define BMP_PLTOP_Harvest				7
#define BMP_PLTOP_Kill					8
#define BMP_PLTOP_Grazing			9
#define BMP_PLTOP_AutoIrrigation	10
#define BMP_PLTOP_AutoFertilizer	11
#define BMP_PLTOP_ReleaseImpound		13
#define BMP_PLTOP_ContinuousFertilizer	14
#define BMP_PLTOP_ContinuousPesticide	15
#define BMP_PLTOP_Burning							16
#define BMP_PLTOP_SKIPYEAR					17

/// Plant management fields
#define BMP_PLTOP_FLD_SUB					"SUBSCENARIO"
#define BMP_PLTOP_FLD_NAME					"NAME"
#define BMP_PLTOP_FLD_LUCC					"LANDUSE_ID"
#define BMP_PLTOP_FLD_SEQUENCE		"SEQUENCE"
#define BMP_PLTOP_FLD_YEAR					"YEAR"
#define BMP_PLTOP_FLD_MONTH				"MONTH"
#define BMP_PLTOP_FLD_DAY					"DAY"
#define BMP_PLTOP_FLD_HUSC					"HUSC"
#define BMP_PLTOP_FLD_MGTOP				"MGT_OP"
#define BMP_PLTOP_FLD_MGT_PRE			"MGT"
//#define BMP_PLTOP_FLD_MGT2I				"MGT2I"
//#define BMP_PLTOP_FLD_MGT3I				"MGT3I"
//#define BMP_PLTOP_FLD_MGT4I				"MGT4"
//#define BMP_PLTOP_FLD_MGT5					"MGT5"
//#define BMP_PLTOP_FLD_MGT6					"MGT6"
//#define BMP_PLTOP_FLD_MGT7					"MGT7"
//#define BMP_PLTOP_FLD_MGT8					"MGT8"
//#define BMP_PLTOP_FLD_MGT9					"MGT9"
//#define BMP_PLTOP_FLD_MGT10I				"MGT10I"

//BMP Type
#define BMP_TYPE_REACH					1
#define BMP_TYPE_AREAL_STRUCTURAL		2
#define BMP_TYPE_AREAL_NON_STRUCTURAL	3

//Reservoir method column name
#define RESERVOIR_FLOW_ROUTING_METHOD_COLUMN_NAME		"METHOD"
#define RESERVOIR_SEDIMENT_ROUTING_METHOD_COLUMN_NAME	"SEDMETHOD"
#define RESERVOIR_NUTRIENT_ROUTING_METHOD_COLUMN_NAME	"NUTMETHOD"

//Reservoir flow routing method
#define RESERVOIR_FLOW_ROUTING_NAME_RATING_CURVE		"RAT_RES"
#define RESERVOIR_FLOW_ROUTING_NAME_DAILY_OUTFLOW		"MDO_RES"
#define RESERVOIR_FLOW_ROUTING_NAME_MONTHLY_OUTFLOW		"MMO_RES"
#define RESERVOIR_FLOW_ROUTING_NAME_ANUNAL_RELEASE_RATE	"AAR_RES"
#define RESERVOIR_FLOW_ROUTING_NAME_TARGET_RELEASE_RATE	"TRR_RES"

#define RESERVOIR_FLOW_ROUTING_NAME_UNKNOWN				-1
#define RESERVOIR_FLOW_ROUTING_RATING_CURVE				0
#define RESERVOIR_FLOW_ROUTING_DAILY_OUTFLOW			1
#define RESERVOIR_FLOW_ROUTING_MONTHLY_OUTFLOW			2
#define RESERVOIR_FLOW_ROUTING_ANUNAL_RELEASE_RATE		3
#define RESERVOIR_FLOW_ROUTING_TARGET_RELEASE_RATE		4

//Reservoir sediment routing method
#define RESERVOIR_SEDIMENT_ROUTING_NAME_MASS_BALANCE	"SMB_RES"

#define RESERVOIR_SEDIMENT_ROUTING_MASS_UNKONWN			-1
#define RESERVOIR_SEDIMENT_ROUTING_MASS_BALANCE			0

//Reservoir column index
#define RESERVOIR_SA_EM_INDEX			4   
#define RESERVOIR_V_EM_INDEX			5
#define RESERVOIR_SA_PR_INDEX			6   
#define RESERVOIR_V_PR_INDEX			7
#define RESERVOIR_INI_S_INDEX			8
#define RESERVOIR_Q_REL_INDEX			9
#define RESERVOIR_k_res_INDEX			10
#define RESERVOIR_OFLOWMN01_INDEX		11 
#define RESERVOIR_OFLOWMX01_INDEX		23 
#define RESERVOIR_METHOD_INDEX			35
#define RESERVOIR_SED_METHOD_INDEX		36	//The method used to do sediment routing
#define RESERVOIR_INI_SC_INDEX			37	//initial sediment concentration, Mg/m**3, default value is 0.03
#define RESERVOIR_NSED_INDEX			38	//equilibrium sediment concentration, Mg/m**3, 
#define RESERVOIR_D50_INDEX				39	//median particle size of the inflow sediment (um), default value is 10
#define RESERVOIR_NUT_METHOD_INDEX		40  //The method used to do nutrient routing

//Crop classification
#define CROP_IDC_WARM_SEASON_ANNUAL_LEGUME	1
#define CROP_IDC_CODE_SEASON_ANNUAL_LEGUME	2
#define CROP_IDC_PERENNIAL_LEGUME							3
#define CROP_IDC_WARM_SEASON_ANNUAL						4
#define CROP_IDC_COLD_SEASON_ANNUAL						5
#define CROP_IDC_PERENNIAL												6
#define CROP_IDC_TREES														7


/// Field index in CropLookup table
#define CROP_PARAM_COUNT				48
/// Index started with 0
/// ICNUM, IDC,BIO_E, HVSTI, BLAI, FRGRW1, LAIMX1, FRGRW2, LAIMX2, DLAI, CHTMX,     0-10
/// RDMX, T_OPT, T_BASE, CNYLD, CPYLD, BN1, BN2, BN3, BP1, BP2,    11-20
/// BP3, WSYF, USLE_C, GSI, VPDFR, FRGMAX, WAVP, CO2HI, BIOEHI, RSDCO_PL,  21-30
/// OV_N, CN2A, CN2B, CN2C, CN2D, FERTFIELD, ALAI_MIN, BIO_LEAF, MAT_YRS, BMX_TREES, 31-40
/// EXT_COEF, BM_DIEOFF, PHU, CNOP, LAI_INIT, BIO_INIT, CURYR_INIT 41-47
#define 	CROP_PARAM_IDX_IDC			1
#define 	CROP_PARAM_IDX_BIO_E		2
#define 	CROP_PARAM_IDX_HVSTI		3
#define 	CROP_PARAM_IDX_BLAI			4
#define 	CROP_PARAM_IDX_FRGRW1	5
#define 	CROP_PARAM_IDX_LAIMX1		6
#define 	CROP_PARAM_IDX_FRGRW2	7
#define 	CROP_PARAM_IDX_LAIMX2		8
#define 	CROP_PARAM_IDX_DLAI			9
#define 	CROP_PARAM_IDX_CHTMX		10
#define 	CROP_PARAM_IDX_RDMX		11
#define 	CROP_PARAM_IDX_T_OPT		12
#define 	CROP_PARAM_IDX_T_BASE	13
#define 	CROP_PARAM_IDX_CNYLD		14
#define 	CROP_PARAM_IDX_CPYLD		15
#define 	CROP_PARAM_IDX_BN1			16
#define 	CROP_PARAM_IDX_BN2	17
#define 	CROP_PARAM_IDX_BN3	18
#define 	CROP_PARAM_IDX_BP1	19
#define 	CROP_PARAM_IDX_BP2	20
#define 	CROP_PARAM_IDX_BP3	21
#define 	CROP_PARAM_IDX_WSYF	22
#define 	CROP_PARAM_IDX_USLE_C	23
#define 	CROP_PARAM_IDX_GSI	24
#define 	CROP_PARAM_IDX_VPDFR	25
#define 	CROP_PARAM_IDX_FRGMAX	26
#define 	CROP_PARAM_IDX_WAVP	27
#define 	CROP_PARAM_IDX_CO2HI	28
#define 	CROP_PARAM_IDX_BIOEHI	29
#define 	CROP_PARAM_IDX_RSDCO_PL	30
#define 	CROP_PARAM_IDX_OV_N	31
#define 	CROP_PARAM_IDX_CN2A	32
#define 	CROP_PARAM_IDX_CN2B	33
#define 	CROP_PARAM_IDX_CN2C	34
#define 	CROP_PARAM_IDX_CN2D	35
#define 	CROP_PARAM_IDX_FERTFIELD	36
#define 	CROP_PARAM_IDX_ALAI_MIN	37
#define 	CROP_PARAM_IDX_BIO_LEAF	38

/// Field index in TillageLookup table
#define TILLAGE_PARAM_COUNT				7
/// ITNUM, EFTMIX, DEPTIL, RRNS, CNOP_CN2, PRC, DSC
#define TILLAGE_PARAM_EFFMIX_IDX		1
#define TILLAGE_PARAM_DEPTIL_IDX		2
#define TILLAGE_PARAM_RRNS_IDX			3
#define TILLAGE_PARAM_CNOP_IDX			4
#define TILLAGE_PARAM_PRC_IDX			5
#define TILLAGE_PARAM_DSC_IDX			6

/// Field index in FertilizerLookup table
#define FERTILIZER_PARAM_COUNT 10
/// IFNUM, FMINN, FMINP, FORGN, FORGP, FNH3N, BACTPDB, BACTLPDB, BACTKDDB, MANURE
#define 	FERTILIZER_PARAM_FMINN_IDX				1
#define 	FERTILIZER_PARAM_FMINP_IDX				2
#define 	FERTILIZER_PARAM_FORGN_IDX				3
#define 	FERTILIZER_PARAM_FORGP_IDX				4
#define 	FERTILIZER_PARAM_FNH3N_IDX				5
#define 	FERTILIZER_PARAM_BACTPDB_IDX			6
#define 	FERTILIZER_PARAM_BATTLPDB_IDX		7
#define 	FERTILIZER_PARAM_BACKTKDDB_IDX	8

/// Field index in LanduseLookup table
#define LANDUSE_PARAM_COUNT 49
#define LANDUSE_PARAM_LANDUSE_ID_IDX	0
#define LANDUSE_PARAM_CN2A_IDX	1
#define LANDUSE_PARAM_CN2B_IDX	2
#define LANDUSE_PARAM_CN2C_IDX	3
#define LANDUSE_PARAM_CN2D_IDX	4
#define LANDUSE_PARAM_ROOT_DEPTH_IDX	5
#define LANDUSE_PARAM_MANNING_IDX	6
#define LANDUSE_PARAM_INTERC_MAX_IDX	7
#define LANDUSE_PARAM_INTERC_MIN_IDX	8
#define LANDUSE_PARAM_SHC_IDX	9
#define LANDUSE_PARAM_SOIL_T10_IDX	10
#define LANDUSE_PARAM_USLE_C_IDX	11
#define LANDUSE_PARAM_PET_FR_IDX	12
#define LANDUSE_PARAM_PRC_ST1_IDX	13
#define LANDUSE_PARAM_PRC_ST2_IDX	14
#define LANDUSE_PARAM_PRC_ST3_IDX	15
#define LANDUSE_PARAM_PRC_ST4_IDX	16
#define LANDUSE_PARAM_PRC_ST5_IDX	17
#define LANDUSE_PARAM_PRC_ST6_IDX	18
#define LANDUSE_PARAM_PRC_ST7_IDX	19
#define LANDUSE_PARAM_PRC_ST8_IDX	20
#define LANDUSE_PARAM_PRC_ST9_IDX	21
#define LANDUSE_PARAM_PRC_ST10_IDX	22
#define LANDUSE_PARAM_PRC_ST11_IDX	23
#define LANDUSE_PARAM_PRC_ST12_IDX	24
#define LANDUSE_PARAM_SC_ST1_IDX	25
#define LANDUSE_PARAM_SC_ST2_IDX	26
#define LANDUSE_PARAM_SC_ST3_IDX	27
#define LANDUSE_PARAM_SC_ST4_IDX	28
#define LANDUSE_PARAM_SC_ST5_IDX	29
#define LANDUSE_PARAM_SC_ST6_IDX	30
#define LANDUSE_PARAM_SC_ST7_IDX	31
#define LANDUSE_PARAM_SC_ST8_IDX	32
#define LANDUSE_PARAM_SC_ST9_IDX	33
#define LANDUSE_PARAM_SC_ST10_IDX	34
#define LANDUSE_PARAM_SC_ST11_IDX	35
#define LANDUSE_PARAM_SC_ST12_IDX	36
#define LANDUSE_PARAM_DSC_ST1_IDX	37
#define LANDUSE_PARAM_DSC_ST2_IDX	38
#define LANDUSE_PARAM_DSC_ST3_IDX	39
#define LANDUSE_PARAM_DSC_ST4_IDX	40
#define LANDUSE_PARAM_DSC_ST5_IDX	41
#define LANDUSE_PARAM_DSC_ST6_IDX	42
#define LANDUSE_PARAM_DSC_ST7_IDX	43
#define LANDUSE_PARAM_DSC_ST8_IDX	44
#define LANDUSE_PARAM_DSC_ST9_IDX	45
#define LANDUSE_PARAM_DSC_ST10_IDX	46
#define LANDUSE_PARAM_DSC_ST11_IDX	47
#define LANDUSE_PARAM_DSC_ST12_IDX	48

/// irrigation source code:
#define IRR_SRC_RCH					1 /// divert water from reach
#define IRR_SRC_RES					2 /// divert water from reservoir
#define IRR_SRC_SHALLOW		3 ///  divert water from shallow aquifer
#define IRR_SRC_DEEP					4 /// divert water from deep aquifer
#define IRR_SRC_OUTWTSD		5 /// divert water from source outside watershed

#endif