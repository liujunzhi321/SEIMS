#pragma once

#include "BMPBase.h"
#include "ManagementOperation.h"
#include <map>
#include "clsRasterData.h"
#include "ArealParameter.h"

#define DISLLOW_COPY_AND_ASSIGN(TypeName)    \
    TypeName(const TypeName &);                \
    void operator = (const TypeName &)

using namespace std;

namespace MainBMP
{
    class BMPArealNonStructural :
            public BMPBase
    {
    private:
        DISLLOW_COPY_AND_ASSIGN(BMPArealNonStructural);

    public:
        BMPArealNonStructural(
                string bmpDatabasePath,
                string parameterTableName,
                int bmpId,
                string bmpName,
                string distribution,
                int scenario);

        ~BMPArealNonStructural(void);

        //get operation for given location and time
        //the startYear is used to decide the time cycling
        NonStructural::ManagementOperation *getOperation(int validCellIndex, int startYear, time_t currentTime);

        //get operation for given location and time
        //the startYear is used to decide the time cycling
        NonStructural::ManagementOperation *getSecondOperation(int validCellIndex, int startYear, time_t currentTime);

        void Dump(ostream *fs);

    private:
        int m_scenarioId;
        string m_distributionMapName;
        string m_distributionTableName;

        //load all the operation from distribution table in bmp database
        void loadOperations();

        //load all the parameters from parameter table in bmp database
        void loadParameters();

        //all the columns to be queried
        string getAllQueryColumns();

        //get parameter for given id
        BMPParameter::ArealParameter *getParameter(int id);

    protected:
        //All the paramters corresponding to the parameter column in scenario table
        //Key is the parameter id
        map<int, BMPParameter::ArealParameter *> m_operationParameters;

        //All the operations involved in this BMP
        //Key is string combined by location,year,month and day.
        //This makes the map structure simple.
        map<string, NonStructural::ManagementOperation *> m_operations;

        //All the operations involved in this BMP
        //Key is string combined by location,year,month and day.
        //This makes the map structure simple.
        map<string, NonStructural::ManagementOperation *> m_secondOperations;

        //max year which has operation
        //used to do time cycling
        int m_maxYear;

        //distribution, used to convert cell to location
    protected:
        //distribution map of current scenario
        clsRasterData *m_currentScearioDistribution;

    };
}


