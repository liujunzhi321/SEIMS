#! /usr/bin/env python
#coding=utf-8
## @Configuration of Preprocessing for SEIMS
# 
#  TODO, give more detailed description here.
import os
## MongoDB related
#hostname = '192.168.6.55'
hostname = '127.0.0.1'
port = 27017
climateDBName = 'climate_dianbu'
## Input
dataprepareDir = 'E:\github-zlj\model_data\model_dianbu_30m_longterm\data_prepare'
PrecSitesVorShp = dataprepareDir + os.sep + 'climate' + os.sep + 'shp' + os.sep + 'Preci_dianbu_Vor.shp'
MeteorSitesVorShp = dataprepareDir + os.sep + 'climate' + os.sep + 'shp' + os.sep + 'Metero_hefei_Vor.shp'

PrecExcelPrefix = dataprepareDir + os.sep + 'climate' + os.sep + 'precipitation_by_day_'
PrecDataYear = [2014]
MeteoVarFile = dataprepareDir + os.sep + 'climate' + os.sep + 'Variables.txt'
MeteoDailyFile = dataprepareDir + os.sep + 'climate' + os.sep + 'meteorology_dianbu_daily.txt'
MetroSiteFile = dataprepareDir + os.sep + 'climate' + os.sep + 'sites_hefei.txt'
