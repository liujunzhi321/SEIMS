#include "ManagementOperationTillage.h"

using namespace MainBMP;
using namespace NonStructural;

ManagementOperationTillage::ManagementOperationTillage(
        int location, int year, int month, int day,
        BMPParameter::ArealParameter *parameter,
        int code) : ManagementOperation(location, year, month, day, parameter)
{
    m_tillCode = code;
}

ManagementOperationTillage::~ManagementOperationTillage(void)
{
}

void ManagementOperationTillage::Dump(ostream *fs)
{
    if (fs == NULL) return;
    ManagementOperation::Dump(fs);
    *fs << "Tillage Code : " << this->m_tillCode << endl;
    if (this->m_parameter != NULL)
    {
        *fs << "*** parameters ***" << endl;
        this->m_parameter->Dump(fs);
    }
}
