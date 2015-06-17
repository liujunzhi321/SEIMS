#! /usr/bin/env python
#coding=utf-8
## @Main function entrance for preprocessing
#
#
from config import *
from PrecipitationDaily import *
from MeteorologicalDaily import *

if __name__ == "__main__":
    ## Climate Data
    ImportDailyMeteoData(hostname,port,climateDBName,MeteoVarFile,MeteoDailyFile,MetroSiteFile)
    ImportDailyPrecData(hostname,port,climateDBName,PrecSitesVorShp,PrecExcelPrefix,PrecDataYear)