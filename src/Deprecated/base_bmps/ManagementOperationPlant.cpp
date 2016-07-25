#include "ManagementOperationPlant.h"

using namespace MainBMP;
using namespace NonStructural;

ManagementOperationPlant::ManagementOperationPlant(
        int location, int year, int month, int day,
        BMPParameter::ArealParameter *parameter,
        int crop) : ManagementOperation(location, year, month, day, parameter)
{
    m_cropType = crop;
}

ManagementOperationPlant::~ManagementOperationPlant(void)
{
}

void ManagementOperationPlant::Dump(ostream *fs)
{
    if (fs == NULL) return;
    ManagementOperation::Dump(fs);
    *fs << "Crop Type : " << this->m_cropType << endl;
    if (this->m_parameter != NULL)
    {
        *fs << "*** parameters ***" << endl;
        this->m_parameter->Dump(fs);
    }
}
