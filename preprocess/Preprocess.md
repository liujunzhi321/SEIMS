# Proprocessing for SEIMS
--------
prepared by Liangjun Zhu

2015/6/17

## 1 Climate Data
Include **Precipitation** and **Meteorological** daily data. 
~~~
preprocess/main.py
--ImportDailyMeteoData(hostname,port,climateDBName,MeteoVarFile,MeteoDailyFile,MetroSiteFile)
--ImportDailyPrecData(hostname,port,climateDBName,PrecSitesVorShp,PrecExcelPrefix,PrecDataYear)
~~~
**TODO**: 降水数据和气象数据的准备形式不同，应尽量简单化，如都用文本文件，逐条存储，并带有表头，用于匹配Variables名。 而且，站点泰森多边形此处并没有什么用处，只是读取了站点信息，应当将站点信息存入文本文件。

## 2 Hydrological Data
Include **Discharge** daily and storm date.
~~~
ImportDailyDischargeData(hostname,port,climateDBName,DischargeExcelPrefix,DischargeYear)
~~~
**TODO**: 流量日值数据的导入和气象数据存在问题一样，需组织成二维矩阵形式的excel表格，非常不方便。

## 3 Spatial Data
