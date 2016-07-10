/*!
 * \file MetadataInfoConst.h
 * \brief Define some const variables used by MetadataInfo class
 *
 * This file contains string constants for the MetadataInfo XML Schema.
 *
 * \author Junzhi Liu
 * \version 1.1
 * \date Jun. 2010
 *
 * 
 */
#pragma once
#include <string>

using namespace std;

//! XML Info file header
const string XMLHeader = "<?xml version='1.0' encoding='utf-8'?>\n<?xml-schema href='InfoXMLSchema.xsd'?>";
//! comment for XML file
const string XMLComment = "<!-- This file was auto-generated by the MetaDataInfo class. -->";
//! XML element tags
const string TagMetadata = "metadata";
const string TagMetadataAttributes =  "version='0.4'";

const string TagClass = "class";
const string TagClassName = "name";
const string TagClassDescription = "description";

const string TagInformation = "information";
const string TagInfoId = "id";
const string TagInfoName = "name";
const string TagInfoDescription = "description";
const string TagInfoVersion = "version";
const string TagInfoAuthor = "author";
const string TagInfoEmail = "email";
const string TagInfoWebsite = "website";
const string TagInfoHelpfile = "helpfile";

const string TagInputs = "inputs";
const string TagInputVariable = "inputvariable";
const string TagInputVariableName = "name";
const string TagInputVariableUnits = "units";
const string TagInputVariableDescription = "description";
const string TagInputVariableSource = "source";
const string TagInputVariableDimension = "dimension";

const string TagOutputs = "outputs";
const string TagOutputVariable = "outputvariable";
const string TagOutputVariableName = "name";
const string TagOutputVariableUnits = "units";
const string TagOutputVariableDescription = "description";
const string TagOutputVariableDimension = "dimension";

const string TagParameters = "parameters";
const string TagParameter = "parameter";
const string TagParameterName = "name";
const string TagParameterUnits = "units";
const string TagParameterDescription = "description";
const string TagParameterSource = "source";
const string TagParameterDimension = "dimension";

const string TagDependencies = "dependencies";

/*!
 * \enum dimensionTypes
 * \ingroup util
 * \brief enum of dimension data types
 *
 */
enum dimensionTypes
{
	/// Unknown type
	DT_Unknown			= -1,
	/// Single numeric
	DT_Single				= 0, 
	/// 1D array, e.g., maximum temperature of sites
	DT_Array1D			= 1,
	/// 2D array
	DT_Array2D			= 2, 
	/// 3D array, currently not used
	DT_Array3D			= 3, 
	/// 1D date array
	DT_Array1DDateValue = 4, 
	/// Raster data, same as DT_Array1D
	DT_Raster1D	        = 5,  
	/// 2D raster, e.g., multi-layers of soil properties
	DT_Raster2D			= 6,
	/// Hydrological or meteorological site information
	DT_SiteInformation	= 7, 
	/// Lapse rate array, TODO need more detail description
	DT_LapseRateArray	= 8, 
	/// Scenario used by BMPs
	DT_Scenario			= 9,
	/// Reach parameters
	DT_Reach				= 10,
	/// Lookup tables
	DT_LookupTable	= 11
};


