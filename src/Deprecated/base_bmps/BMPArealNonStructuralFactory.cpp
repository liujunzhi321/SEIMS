#include "BMPArealNonStructuralFactory.h"

using namespace MainBMP;

BMPArealNonStructuralFactory::BMPArealNonStructuralFactory(int scenarioId, int bmpId,
                                                           int bmpType, string distribution, string parameter)
        : BMPFactory(scenarioId, bmpId, bmpType, distribution, parameter)
{
    m_bmpArealNonStructural = NULL;
}


BMPArealNonStructuralFactory::~BMPArealNonStructuralFactory(void)
{
    if (m_bmpArealNonStructural != NULL) delete m_bmpArealNonStructural;
}

void BMPArealNonStructuralFactory::loadBMP(string bmpDatabasePath)
{
    if (m_bmpType != BMP_TYPE_AREAL_NON_STRUCTURAL)
        throw ModelException("BMPArealNonStructuralFactory", "loadBMP", "Error BMP type for BMP reach!");

    m_bmpArealNonStructural = new BMPArealNonStructural(bmpDatabasePath, m_parameter, m_bmpId, "", m_distribution,
                                                        m_scenarioId);
}

void BMPArealNonStructuralFactory::Dump(ostream *fs)
{
    if (m_bmpArealNonStructural != NULL)
        m_bmpArealNonStructural->Dump(fs);
}


