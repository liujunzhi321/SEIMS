/*!
 * \file BMPBase.cpp
 * \brief Base class for BMP
 *
 * 
 */
#pragma once

#include <string>
#include <map>
#include <vector>
#include "BMPText.h"
#include "util.h"
#include "utils.h"
#include "ModelException.h"
#include <sstream>
#include <ostream>
#include <iomanip>
#include "MongoUtil.h"

using namespace std;
/** \defgroup bmps
 * \ingroup Base
 * \brief BMP related module
 */
/*!
 *  \addtogroup MainBMP
 *  \ingroup bmps
 */
namespace MainBMP
{
    /*!
     * \class BMPBase
     * \ingroup MainBMP
     *
     * \brief The base class of all BMPs in BMP database
     ** One scenario may have different types of BMPs. And each type of BMP
     ** maybe a single one or a collection of same kind of BMP.
     */
    class BMPBase
    {
    public:
        /*
        ** @brief Constructor for a new bmp database
        **
        ** @param string bmpDatabasePath	The path of the BMP database
        ** @param int id					The BMP id, used to decide how to load distribution and parameter
        ** @param string name				The BMP name, just used for exception information
        ** @param int type					The BMP type id, used to decide how to load distribution and parameter
        ** @param string distribution		The distribution of BMP, asc file or table name
        ** @param string parameter			The parameter table name of BMP, table name
        */
        BMPBase(string bmpDatabasePath, int id, string name, int type, string parameter);

        /*!
         * \brief Constructor for a new bmp database
         *
         * \param[in] conn MongoClient which contains the BMP database
         * \param[in] int The BMP id, used to decide how to load distribution and parameter
         * \param[in] name The BMP name, just used for exception information
         * \param[in] type The BMP type id, used to decide how to load distribution and parameter
         * \param[in] distribution The distribution of BMP, tif file or table name stored in MongoDB
         * \param[in] parameter The parameter table name of BMP, table name, stored as GridFs in MongoDB
         */
        BMPBase(mongoc_client_t *conn, int id, string name, int type, string parameter);

        /// Output to stream
        virtual void Dump(ostream *fs);

        /// Get reach BMP name from BMP ID
        static string ReachBMPColumnNameFromBMPID(int reachbmpId);

        /// Destructor
        ~BMPBase(void);

    protected:
        /// BMP Type
        int m_bmpType;
        /// BMP ID
        int m_bmpId;
        /// BMP Name
        string m_bmpName;
        /// mongo client
        mongoc_client_t *m_conn;
        /// SQLite database path
        string m_bmpDatabasePath;
        /// parameter table name stored in database
        string m_parameterTableName;
    };
}



