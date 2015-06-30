/***************************************************************************
* reclassify.cpp
*
* Project: GPRO, v 1.0
* Purpose: Perform reclassification. 
*
* Usage:  
*
* Example: 
*
* Author:   Zhan Lijun
* E-mail:   zhanlj@lreis.ac.cn
*
* Modified: Junzhi Liu
* E-mail:   liujz@lreis.ac.cn
* Data:     2013-05-12
****************************************************************************/


#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <sstream>
#include <omp.h>
#include "mpi.h"
#include "neighborhood.h"
#include "cellSpace.h"
#include "basicTypes.h"
#include "rasterLayer.h"
#include "application.h"
#include "ReclassifyOperator.h"
#include "communication.h"

#ifdef linux
    #define SEP "/"
#else
    #define SEP "\\"
#endif

using namespace std;
using namespace GPRO;

int main(int argc, char *argv[]) 
{
	/*  enum ProgramType{MPI_Type = 0,
				   MPI_OpenMP_Type,
				   CUDA_Type,
				   Serial_Type};*/
	Application::START(MPI_Type, argc, argv); //init
	
	char* configFile;
	char* neighborfile;
	
	if (argc < 3)
    {
        cout << "Usage of reclassify:\n\treclassify configFile neighborhoodFile\n"; 
		cout << "\nFormat of the config file: \n\n";
		cout << "typeLayer defaultType\n";
		cout << "lookupFolder\n";
		cout << "outputFolder\n";
		cout << "numOutputs\n";
		cout << "attribute1\n";
		cout << "attribute2\n";
		cout << "...\n\n";
		cout << "NOTE: No space is allowed in the filename currently.\n";

		exit(-1);
    }
    else
	{
		configFile = argv[1];
        neighborfile = argv[2];
	}

	//omp_set_num_threads(threadNUM);

	// read input information
	string typeFile, lookupFolder, outputFolder, tmp;
	int n = 0;
	int defaultType = -1;
	vector<string> attrNames;

	ifstream ifs(configFile);
	ifs >> typeFile >> defaultType >> lookupFolder >> outputFolder >> n;
	for (int i = 0; i < n; ++i)
	{
		ifs >> tmp;
		attrNames.push_back(tmp);
	}
	ifs.close();

	int nLen = lookupFolder.length();
	if (lookupFolder.substr(nLen-1, 1) != SEP)
		lookupFolder += SEP;
    nLen = outputFolder.length();
	if (outputFolder.substr(nLen-1, 1) != SEP)
		outputFolder += SEP;
	
	RasterLayer<float> typeLayer("Type");
	typeLayer.readNeighborhood(neighborfile);
	typeLayer.readFile(typeFile.c_str());

	// loop to reclassify each attribute
	string lookupFile, outputFile;
	for (int i = 0; i < n; ++i)
	{
		lookupFile = lookupFolder + attrNames[i] + ".txt";
		outputFile = outputFolder + attrNames[i] + ".tif";
		RasterLayer<float> outputLayer("Attribute");
		outputLayer.copyLayerInfo(typeLayer);

		//double starttime = MPI_Wtime();

		MPI_Barrier(MPI_COMM_WORLD);
		//starttime = MPI_Wtime();
		ReclassifyOperator reclassOper;

		reclassOper.SetTypeLayer(typeLayer);	
		reclassOper.SetOutputLayer(outputLayer);
		reclassOper.ReadReclassMap(lookupFile.c_str());
		reclassOper.SetDefualtType(defaultType);
		reclassOper.Run();

		MPI_Barrier(MPI_COMM_WORLD);

		//double endtime = MPI_Wtime();

		//cout<<"run time is "<<endtime-starttime<<endl;
		outputLayer.writeFile(outputFile.c_str());
	}
	
	Application::END();
	return 0;
}
