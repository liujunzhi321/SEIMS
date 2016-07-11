#pragma once

#include "BMPFactory.h"
#include "BMPArealNonStructural.h"

namespace MainBMP
{
    class BMPArealNonStructuralFactory :
            public BMPFactory
    {
    public:
        BMPArealNonStructuralFactory(int scenarioId, int bmpId, int bmpType, string distribution, string parameter);

        ~BMPArealNonStructuralFactory(void);

        void loadBMP(string bmpDatabasePath);

        void Dump(ostream *fs);

    private:
        BMPArealNonStructural *m_bmpArealNonStructural;
    };
}


