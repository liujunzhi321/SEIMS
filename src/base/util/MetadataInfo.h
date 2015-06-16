/*!
 * \file MetadataInfo.h
 * \brief Define MetadataInfo class used by modules
 *
 *
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date June 2010
 *
 * 
 */
#pragma once

#include <sstream>
#include <string>
#include <vector>
#include "MetadataInfoConst.h"

using namespace std;


/*!
 * \ingroup Util
 * \class ModelClass
 *
 * \brief Class for model
 *
 *
 *
 */
class ModelClass
{
public:
	string Name;
	string Description;
};
/*!
 * \ingroup Util
 * \class Parameter
 *
 * \brief Model parameter information class
 *
 *
 *
 */
class Parameter
{
public:
	string Name;
	string Units;
	string Description;
	string Source;
	dimensionTypes Dimension;///< \a dimensionTypes
};
/*!
 * \ingroup Util
 * \class Information
 *
 * \brief Module development information class
 *
 *
 *
 */
class Information
{
public:
	string Id;
	string Name;
	string Description;
	string Version;
	string Author;
	string EMail;
	string Website;
	string Helpfile;
};
/*!
 * \ingroup Util
 * \class InputVariable
 *
 * \brief Input variable information class
 *
 *
 *
 */
class InputVariable
{
public:
	string Name;
	string Units;
	string Description;
	string Source;
	dimensionTypes Dimension;///< \a dimensionTypes
};
/*!
 * \ingroup Util
 * \class OutputVariable
 *
 * \brief Output variable information class
 *
 *
 *
 */
class OutputVariable
{
public:
	string Name;
	string Units;
	string Description;
	dimensionTypes Dimension;///< \a dimensionTypes
};
/*!
 * \ingroup Util
 * \class MetadataInfo
 *
 * \brief Metadata information of module
 *
 *
 *
 */
class MetadataInfo
{
private:
	//! private member variables
	 string m_strSchemaVersion;			//! latest XML schema version supported by this class
	 ModelClass m_oClass;				//! class name for the module
	 Information m_Info;				//! the general information for the module
	 vector<InputVariable> m_vInputs;		//! list of input parameters for the module
	 vector<OutputVariable> m_vOutputs;	//! list of output parameters for the module
	 vector<ModelClass> m_vDependencies;	//! list of dependency classes for the module
	 vector<Parameter> m_vParameters;		//! list of parameters for the module

    void OpenTag(string name, string attributes, int indent, string* sb);
    void CloseTag(string name, int indent, string* sb);
    void FullTag(string name, int indent, string content, string* sb);
    void WriteClass(int indent, string* sb);
    void WriteInformation(int indent, string* sb);
    void WriteInputs(int indent, string* sb);
    void WriteOutputs(int indent, string* sb);
	void WriteParameters(int indent, string* sb);
    void WriteDependencies(int indent, string* sb);
    void WriteXMLHeader(string* sb);
	void DimensionTag(string tag, int indent, dimensionTypes dimType, string* sb);

public:
	MetadataInfo(void);
	~MetadataInfo(void);

	string SchemaVersion();

	void SetClass(string name, string description);

	string GetClassName();
	string GetClassDescription();

	void SetID(string ID);
	string GetID();
	void SetName(string name);
	string GetName();
	void SetDescription(string description);
	string GetDescription();
	void SetVersion(string version);
	string GetVersion();
	void SetAuthor(string author);
	string GetAuthor(void);
	void SetEmail(string email);
	string GetEmail();
	void SetWebsite(string site);
	string GetWebsite();
	void SetHelpfile(string file);
	string GetHelpfile();

	int GetInputCount();
	int AddInput(string name, string units, string desc, string source, dimensionTypes dimType);
	string GetInputName(int index);
	string GetInputUnits(int index);
	string GetInputDescription(int index);
	string GetInputSource(int index);
	dimensionTypes GetInputDimension(int index);
	InputVariable GetInput(int index);

	int GetOutputCount();
	int AddOutput(string name, string units, string desc, dimensionTypes dimType);
	string GetOutputName(int index);
	string GetOutputUnits(int index);
	string GetOutputDescription(int index);
	dimensionTypes GetOutputDimension(int index);
	OutputVariable GetOutput(int index);

	int GetParameterCount();
	int AddParameter(string name, string units, string desc, string source, dimensionTypes dimType);
	string GetParameterName(int index);
	string GetParameterUnits(int index);
	string GetParameterDescription(int index);
	string GetParameterSource(int index);
	dimensionTypes GetParameterDimension(int index);
	Parameter GetParameter(int index);

	int GetDependencyCount();
	int AddDependency(string name, string description);
	string GetDependencyName(int index);
	string GetDependencyDescription(int index);
	ModelClass GetDependency(int index);

	string GetXMLDocument();
};
