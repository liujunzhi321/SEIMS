/*!
 * \file BMPPointSourceFactory.h
 * \brief Point source pollution and BMP factory
 * \author Liang-Jun Zhu
 * \date July 2016
 * 
 */
#pragma once
#include <stdlib.h>
#include "BMPFactory.h"
using namespace MainBMP;

namespace MainBMP
{
	/*!
	 * \class PointBMPLocations
	 * \ingroup MainBMP
	 *
	 * \brief Base class of point BMP, mainly store location related parameters
	 *
	 */
	class PointBMPLocations
	{
	public:
		/*!
		 * \brief Constructor, parse point BMP location related parameters from bson object
		 * \param[in] bsonTab Query result from MongoDB
		 * \param[in] iter Iterator of bsonTab
		 */
		PointBMPLocations(const bson_t *&bsonTab, bson_iter_t &iter);
		/// Destructor
		~PointBMPLocations(void);
		/// Output
		void Dump(ostream* fs);
		/// Get point source ID
		int GetPointSourceID(){return m_pointSrcID;}
		/// name
		string GetPointSourceName(){return m_name;}
		/// Lat
		float GetLat(){return m_lat;}
		/// Lon
		float GetLon(){return m_lon;}
		/// localX
		float GetLocalX(){return m_localX;}
		/// localY
		float GetLocalY(){return m_localY;}
		/// Located subbasin ID
		int GetSubbasinID(){return m_subbasinID;}
		/// size
		float GetSize(){return m_size;}
		/// Distance to the downstream reach
		float GetDistanceDown(){return m_distDown;}
	private:
		/// ID of point source
		int m_pointSrcID;
		/// name
		string m_name;
		/// Lat
		float m_lat;
		/// Lon
		float m_lon;
		/// localX
		float m_localX;
		/// localY
		float m_localY;
		/// Located subbasin ID
		int m_subbasinID;
		/// size
		float m_size;
		/// Distance to the downstream reach
		float m_distDown;
	};
	/*!
	 * \class PointSourceParameters
	 * \ingroup MainBMP
	 *
	 * \brief Point source management parameters
	 *
	 */
	class PointSourceMgtParams
	{
	public:
		/*!
		 * \brief Constructor, parse point source management parameters from bson object
		 * \param[in] bsonTab Query result from MongoDB
		 * \param[in] iter Iterator of bsonTab
		 */
		PointSourceMgtParams(const bson_t *&bsonTab, bson_iter_t &iter);
		/// Destructor
		~PointSourceMgtParams(void);
		/// Output
		void Dump(ostream* fs);
		/// Get sequence number
		int GetSequence(){return m_seqence;}
		/// Get subScenario name
		string GetSubScenarioName(){return m_name;}
		/// Get water volume
		float GetWaterVolume(){return m_waterVolume;}
		/// Get sediment concentration
		float GetSedment(){return m_sedimentConc;}
		/// Get sediment concentration
		float GetTN(){return m_TNConc;}
		/// Get NO3 concentration
		float GetNO3(){return m_NO3Conc;}
		/// Get NH3 concentration
		float GetNH3(){return m_NH3Conc;}
		/// Get OrgN concentration
		float GetOrgN(){return m_OrgNConc;}
		/// Get TP concentration
		float GetTP(){return m_TPConc;}
		/// Get MinP concentration
		float GetMinP(){return m_MinPConc;}
		/// Get OrgP concentration
		float GetOrgP(){return m_OrgPConc;}
	private:
		/// subSecenario name
		string m_name;
		/// Sequence number of management
		int m_seqence;
		/// Start date
		time_t m_startDate;
		/// End date
		time_t m_endDate;
		///  Q	Water volume	m3/'size'/day ('Size' may be one cattle or one pig, depends on PTSRC code)
		float m_waterVolume;
		/// Sed	Sediment concentration	kg/'size'/day
		float m_sedimentConc;
		///	TN	Total Nitrogen concentration	kg/'size'/day
		float m_TNConc;
		///	NO3	Nitrate Nitrogen concentration	kg/'size'/day
		float m_NO3Conc;
		/// NH3	Ammonium Nitrogen concentration	kg/'size'/day
		float m_NH3Conc;
		///	ORGN	Organic Nitrogen concentration	kg/'size'/day
		float m_OrgNConc;
		///	TP	Total phosphorus concentration	kg/'size'/day
		float m_TPConc;
		///	MINP	Mineral phosphorus concentration	kg/'size'/day
		float m_MinPConc;
		///	ORGP	Organic phosphorus concentration	kg/'size'/day
		float m_OrgPConc;
	};
	/*!
	 * \class BMPPointSrcFactory
	 * \ingroup MainBMP
	 *
	 * \brief Base class of point source BMPs.
	 * Actually, include point pollution sources, such as sewage outlet of animal farm.
	 *
	 */
	class BMPPointSrcFactory: public BMPFactory
	{
	public:
		/// Constructor
		BMPPointSrcFactory(int scenarioId,int bmpId,int subScenario,int bmpType,int bmpPriority,string distribution,string collection, string location);
		/// Destructor
		~BMPPointSrcFactory(void);
		/// Load BMP parameters from MongoDB
		void loadBMP(mongoc_client_t* conn, string& bmpDBName);
		/// Output
		void Dump(ostream* fs);
		/*!
		 * \brief Load point BMP location related parameters from MongoDB
		 * \param[in] conn mongoc client instance
		 * \param[in] bmpDBName BMP Scenario database
		 */
		void ReadPointSourceManagements(mongoc_client_t* conn, string& bmpDBName);
		/*!
		 * \brief Load point BMP location related parameters from MongoDB
		 * \param[in] conn mongoc client instance
		 * \param[in] bmpDBName BMP Scenario database
		 */
		void ReadPointBMPLocations(mongoc_client_t* conn, string& bmpDBName);
	private:
		/// Code of point source
		int m_pointSrc;
		/// Collection of point source management parameters
		string m_pointSrcMgtTab;
		/// Sequences of point source managements
		vector<int> m_pointSrcMgtSeqs;
		/* Map of point source management parameters
		 * Key: Scheduled sequence number, unique
		 * Value: Pointer of PointBMPParamters instance
		 */
		map<int, PointSourceMgtParams*> m_pointSrcMgtMap;
		/// Collection of point source locations
		string m_pointSrcDistTab;
		/// IDs of point source of current subScenario
		vector<int> m_pointSrcIDs;
		/* Map of point source BMP location related parameters
		 * Key: PTSRCID, unique
		 * Value: Pointer of PointBMPParamters instance
		 */
		map<int, PointBMPLocations*> m_pointSrcLocsMap;
	};
}