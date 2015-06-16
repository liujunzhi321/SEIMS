/*!
 * \file SEIMS_ModuleForModule.h
 * \brief
 *
 *
 *
 * \author [your name]
 * \version 
 * \date June 2015
 *
 * 
 */
/*!
 * \file SEIMS_ModuleForModule.h
 * \brief
 *
 *
 *
 * \author [your name]
 * \version 
 * \date June 2015
 *
 * 
 */
#pragma once

#include <time.h>
#include "MetadataInfo.h"
#include <string>
#include "Scenario.h"
#include "SEIMS_ModuleSetting.h"

using namespace MainBMP;
using namespace std;

//typedef void __cdecl (*pICStatusMessage)(const char* );
typedef void (*pICStatusMessage)(const char* );
//! \namespace Module
namespace Module
{
	/*!
	 * \ingroup module_setting
	 * \class SEIMSModule
	 *
	 * \brief This is the basic class for all the modules. 
	 *
	 *
	 *
	 */
	class SEIMSModule
	{
	public:
		SEIMSModule(void);
		~SEIMSModule(void);

		//! virtual functions
		bool virtual SetValue (const char* key, float data){return true;}
		bool virtual Set1DData(const char* key, int nRows, float* data){return true;}
		bool virtual Set2DData(const char* key, int nRows, int nCols, float** data){return true;}
		bool virtual Set3DData(const char* key, int nRows1d, int* nRows2d,int* nRows3d,float***data){return true;}
		bool virtual SetScenario(Scenario* scen){return true;}
		bool virtual Get1DData(const char* key, int* nRows, float** data) {return true;}
		bool virtual Get2DData(const char* key, int *nRows, int *nCols, float*** data){return true;}
		bool virtual Execute(void) = 0;
		//bool virtual InitModule(SEIMSModuleSetting* setting) = 0;
		//bool virtual EndModule() = 0;

		string virtual ModuleName(void) = 0;
		string virtual ModuleAuthor(void) = 0;
		string virtual ModuleDescription(void) = 0;
		string virtual ModuleVersion(void) = 0;
		string virtual ModuleClassName(void) = 0;

		//common functions
		bool SetDate(time_t date);
		void SetStatusCallback(pICStatusMessage cb);
		string MetadataInformation(void);
	protected:
		void CreateBasicMetadataInfo(void);
	protected:
		time_t				m_date;
		pICStatusMessage	m_StatusMsg;
		MetadataInfo*	    m_metadataInfo;	
	protected:
		/**
		*	@brief check the input data. Make sure all the input data is available.
		*
		*	@return bool The validity of the input data.
		*/
		bool virtual CheckInputData(void) = 0;

		/**
		*	@brief check the input size. Make sure all the input data have same dimension.
		*	
		*	@param key: The key of the input data
		*	@param sizeFromMain: Data dimension come from main program
		*	@param sizeInModule: Data dimension saved in module
		*	@return bool The validity of the dimension
		*/
		bool CheckInputSize(const char*key ,int sizeFromMain, int* sizeInModule);

		void StatusMsg(string msg);

	};
}


