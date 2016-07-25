#include "ManagementOperation.h"
#include <sstream>
#include <iomanip>

using namespace MainBMP;
using namespace NonStructural;

ManagementOperation::ManagementOperation(int location, int year, int month, int day,
                                         BMPParameter::ArealParameter *parameter) :
        m_location(location), m_year(year), m_month(month), m_day(day), m_parameter(parameter)
{
}

ManagementOperation::~ManagementOperation(void)
{
}

string ManagementOperation::UniqueID()
{
    return ManagementOperation2ID(this->m_location, this->m_year, this->m_month, this->m_day);
}

string ManagementOperation::ManagementOperation2ID(int location, int year, int month, int day)
{
    ostringstream os;
    os << setw(8) << setfill('0') << location << "|"
    << setw(4) << setfill('0') << year << "|"
    << setw(2) << setfill('0') << month << "|"
    << setw(2) << setfill('0') << day;
    return os.str();
}

void ManagementOperation::Dump(ostream *fs)
{
    if (fs == NULL) return;
    *fs << "operation:" << UniqueID() << endl;
}