#include "ManagementOperationHarvest.h"

using namespace MainBMP;
using namespace NonStructural;

ManagementOperationHarvest::ManagementOperationHarvest(
        int location, int year, int month, int day, BMPParameter::ArealParameter *parameter) :
        ManagementOperation(location, year, month, day, parameter)
{
}

ManagementOperationHarvest::~ManagementOperationHarvest(void)
{
}
