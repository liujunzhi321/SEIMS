#pragma once

#include <string>
#include <vector>
#include <map>

#include "mongo.h"

using namespace std;

struct Subbasin
{
    float qOutlet; // flow out the subbasin outlet
    bool calculated; // whether this subbasin is already calculated
    int rank;
    Subbasin *downStream;
    int disToOutlet;
    int id;
    int group;
    vector<Subbasin *> upStreams;

    Subbasin(int id, int group) : qOutlet(0.f), calculated(false), rank(-1), downStream(NULL), disToOutlet(-1)
    {
        this->id = id;
        this->group = group;
    }
};

int ReadInputData(int index, string &layerFilePrefix, string &dirFilePrefix,
                  string &slopeFilePrefix, string &streamFilePre,
                  float **&rtLayers, float **&flowIn, float *&flowOut, float *&dir, float *&slope, float *&stream,
                  int &layersCount, int &cellsCount);

int ReadSubbasinOutlets(const char *outletFile, int nSubbasins, float **&outlets);

int ReadRiverTopology(const char *reachFile, map<int, Subbasin *> &subbasins, set<int> &groupSet);

int GetGroupCount(mongo *conn, const char *dbName, int decompostionPlan);

int ReadTopologyFromMongoDB(mongo *conn, const char *dbName, map<int, Subbasin *> &subbasins, set<int> &groupSet,
                            int decompostionPlan, const char *groupField);
