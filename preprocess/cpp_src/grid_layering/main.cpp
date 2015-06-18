/*----------------------------------------------------------------------
*	Purpose: 	Grid layering functions
*
*	Created:	Junzhi Liu
*	Date:		28-March-2013
*
*	Revision:
*   Date:
*---------------------------------------------------------------------*/


#include <iostream>
#include <sstream>
#include <ctime>
#include "GridLayering.h"
#include "gridfs.h"
#include "mongo.h"
#include "bson.h"

using namespace std;

int main(int argc, char** argv)
{
    if(argc < 5)
    {
        cout << "usage: grid_layering hostname port output_dir modelname nsubbasin\n";
        exit(-1);
    }

    const char* hostName = argv[1];
    int port = atoi(argv[2]);
    const char* outputDir = argv[3];
    const char* modelName = argv[4];
    int nSubbasins = atoi(argv[5]);

    mongo conn[1];
    gridfs gfs[1];
    int status = mongo_connect(conn, hostName, port); 
    if( MONGO_OK != status ) 
    {
        cout << "can not connect to mongodb.\n";
        exit(-1);
    }

    gridfs_init(conn, modelName, "spatial", gfs); 

	int outputNoDataValue = -9999;
	clock_t t1 = clock();

    for (int i = 1; i <= nSubbasins; i++)
    {
        ostringstream oss;
        oss << i << "_FLOW_DIR";
        RasterHeader header;
        int *dirMatrix;
        ReadFromMongo(gfs, oss.str().c_str(), header, dirMatrix); 

	    int nRows = header.nRows;
	    int nCols = header.nCols;
	    int dirNoDataValue = header.noDataValue;

        int n = nRows*nCols;
        int *compressedIndex = new int[n];
        int nValidGrids = CalCompressedIndex(n, dirMatrix, header.noDataValue, compressedIndex);

        OutputFlowOutD8(gfs, i, nRows, nCols, nValidGrids, dirMatrix, header.noDataValue, compressedIndex);

	    string layeringFile = LayeringFromSourceD8(outputDir, gfs, i, nValidGrids, dirMatrix, compressedIndex, header, outputNoDataValue);
        cout << layeringFile << endl;
        OutputLayersToMongoDB(layeringFile.c_str(), "ROUTING_LAYERS_UP_DOWN", i, gfs);

        // The following code is for d-infinite algorithm
        ostringstream ossDinf;
        ossDinf << i << "_FLOW_DIR_DINF";
        int *dirMatrixDinf;
        ReadFromMongo(gfs, ossDinf.str().c_str(), header, dirMatrixDinf); 

        ostringstream ossAngle;
        ossAngle<< i << "_FLOW_DIR_ANGLE_DINF";
        float *angle;
        ReadFromMongoFloat(gfs, ossAngle.str().c_str(), header, angle); 

        float *flowOutDinf;
        int* outDegreeMatrixDinf = GetOutDegreeMatrix(dirMatrixDinf, nRows, nCols, dirNoDataValue);
        int nOutputFlowOut = OutputMultiFlowOut(nRows, nCols, nValidGrids, outDegreeMatrixDinf, dirMatrixDinf, dirNoDataValue, compressedIndex, flowOutDinf);;
        WriteStringToMongoDB(gfs, i, "FLOWOUT_INDEX_DINF", nOutputFlowOut, (const char*)flowOutDinf);

	    string layeringFileDinf = LayeringFromSourceDinf(outputDir, gfs, i, nValidGrids, angle, dirMatrixDinf, compressedIndex, header, outputNoDataValue);
        cout << layeringFileDinf << endl;
        OutputLayersToMongoDB(layeringFileDinf.c_str(), "ROUTING_LAYERS_DINF", i, gfs);


	    delete dirMatrix;
        delete compressedIndex;

        delete dirMatrixDinf;
        delete outDegreeMatrixDinf;
    }

	clock_t t2 = clock();

	//cout << t2 - t1 << endl;
	return 0;
}
