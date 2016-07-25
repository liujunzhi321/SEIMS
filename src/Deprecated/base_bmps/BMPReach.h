#pragma once

#include "BMPBase.h"

namespace MainBMP
{
    class BMPReach : public BMPBase
    {
    public:
        BMPReach(string bmpDatabasePath,        /*The path of BMP database*/
                 string parameterTableName,    /*The parameter table name*/
                 int bmpId,                    /*The BMP id in BMP_index table*/
                 string bmpName,                /*The reach id in Reach_BMP table*/
                 int reachId,                /*Reach ID, i.e. distribution*/
                 int reachStructurId);

        /*Reach structure ID*/

        ~BMPReach(void);

        virtual void Dump(ostream *fs);

        //conversion between time series data name and type
        static int TimeSeriesDataName2Type(string name);

        static string TimeSeriesDataType2Name(int type);

        //public information
        time_t OperationDate() { return m_operationDate; }

        int ID() { return m_reachStructureId; }

    protected:
        /*
        ** Check if the parameter table of reach constructure is valid.
        */
        //virtual bool isReachConstrutureTableValid();

        //distribution
    protected:
        /*
        ** The reach id
        */
        int m_reachId;

        /*
        ** The reach structure id
        */
        int m_reachStructureId;

        /*
        ** operation date
        */
        time_t m_operationDate;

        /*
        ** Coordinate
        */
        float m_x;
        float m_y;

        //parameter, loaded by loadParameters()
    protected:

        /*
        ** The length of the properties
        */
        int m_numericParameterNum;

        /*
        ** The data row in one BMP table
        */
        float *m_numericParameters;

        /*
        ** All the text properties
        */
        map<string, string> m_textParameters;
        //time series data
    protected:
        /*
        ** All the time series data connected with reach structure
        ** For MDO_RES,MMO_RES,PointSource and flowDiversion
        */
        map<int, map<time_t, float> > m_timeSerieseData;

    public:
        /*
        ** The column name in Reach_BMP table
        */
        virtual string ColumnName() = 0;

        //Three data loading functions
    private:
        /*
        ** Load parameters from parameter table for given reach structure
        */
        void loadParameters(string parameterTableName, int reachStructurId);

    protected:
        /*
        ** Load specific parameters for different reach structure
        ** Just for reservoir
        */
        virtual void loadStructureSpecificParameter();

    public:
        /*
        ** Load time series data for some reach structure
        */
        void loadTimeSeriesData(string databasePath, time_t startTime, time_t endTime, int interval);

        float getTimeSeriesData(int dataType, time_t t);

    private:
        /*
        ** The table name in hydroclimate database to for reach structure
        */
        string TimeSeriesDataTableName();

        /*
        ** Whether the reach structure has time series data
        ** Just for MDO_RES,MMO_RES,PointSource and flowDiversion
        */
        virtual bool HasTimeSeriesData();

    protected:
        /*
        ** The FILE or TABLENAME column of reach structure
        */
        string DataSource();
    };
}


