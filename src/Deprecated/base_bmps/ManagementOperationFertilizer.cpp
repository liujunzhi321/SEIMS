#include "ManagementOperationFertilizer.h"

using namespace MainBMP;
using namespace NonStructural;

ManagementOperationFertilizer::ManagementOperationFertilizer(
        int location, int year, int month, int day,
        BMPParameter::ArealParameter *parameter,
        int type, int rate) : ManagementOperation(location, year, month, day, parameter)
{
    this->m_ferType = type;
    this->m_ferRate = rate;
}

ManagementOperationFertilizer::~ManagementOperationFertilizer(void)
{
}

void ManagementOperationFertilizer::Dump(ostream *fs)
{
    if (fs == NULL) return;
    ManagementOperation::Dump(fs);
    *fs << "Fertilizer Type : " << this->m_ferType << endl;
    *fs << "Fertilize Rate : " << this->m_ferRate << endl;
    if (this->m_parameter != NULL)
    {
        *fs << "*** parameters ***" << endl;
        this->m_parameter->Dump(fs);
    }
}