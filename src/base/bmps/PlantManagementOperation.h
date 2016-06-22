#pragma once
#include <string>
#include <vector>

using namespace std;
namespace MainBMP
{
	/*!
	 *  \addtogroup PlantManagement
	 * \ingroup MainBMP
	 * \brief Plant management related operations
	 */
	namespace PlantManagement
	{
		/*!
		 * \class PlantManagementOperation
		 * \ingroup PlantManagement
		 * \brief Base class of plant management operation
		 */
		class PlantManagementOperation
		{
		public:
			/*!
			 * \brief Constructor
			 * \param[in] mgtCode 1 to 16
			 * \param[in] husc Fraction of heat units (base or plant)
			 * \param[in] year Rotation year, e.g., 1,2,...
			 * \param[in] month
			 * \param[in] day
			 * \param[in] location Indexes of field to practice the operation (string)
			 */
			PlantManagementOperation(int mgtOp, float husc, int year, int month, int day,float* parameters);
			//! Destructor
			~PlantManagementOperation(void);
			//! Output
			void dump(ostream *fs){};
		protected:
			/// husc
			float m_frHU;
			/// year
			int m_year;
			/// month
			int m_month;
			/// day
			int m_day;
			/// management operation code
			int m_mgtOp;
			/// parameters, mgt1~mgt10
			float* m_parameters;
		};
		/*!
		 * \class PlantOperation
		 * \ingroup PlantManagement
		 * \brief Plant management operation
		 */
		class PlantOperation: public PlantManagementOperation
		{
		public:
			PlantOperation(int mgtOp, float husc, int year, int month, int day,float* parameters);
			~PlantOperation();
			int PlantID(){return m_plantID;}
			int CurYearMaturity(){return m_curYrMat;}
			float HeatUnits(){return m_heatUnits;}
			float LAIInit(){return m_laiInit;}
			float BIOInit(){return m_bioInit;}
			float HITarg(){return m_hiTarg;}
			float BIOTarg(){return m_bioTarg;}
			float CNOP(){return m_CNOP;}
			//! Output
			void dump(ostream *fs);
		private:
			int m_plantID;
			int m_curYrMat;
			float m_heatUnits;
			float m_laiInit;
			float m_bioInit;
			float m_hiTarg;
			float m_bioTarg;
			float m_CNOP;
		};
	}
}



