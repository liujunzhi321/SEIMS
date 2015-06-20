# Proprocessing for SEIMS
--------
prepared by Liangjun Zhu

2015/6/17
## 0 Prepared Data
像所有的水文模型一样，我们的准备数据有气象、水文、空间数据等，下图先入为主，展示了输入数据的文件结构。
![prepareddata](../doc/img/prepareddata.png)
图1 模型数据准备

## 1 Climate Data
Include **Precipitation** and **Meteorological** daily data. 
~~~
preprocess/main.py
--ImportDailyMeteoData(hostname,port,climateDBName,MeteoVarFile,MeteoDailyFile,MetroSiteFile)
--ImportDailyPrecData(hostname,port,climateDBName,PrecSitesVorShp,PrecExcelPrefix,PrecDataYear)
~~~
**TODO**: 
+ 降水数据和气象数据的准备形式不同，应尽量简单化，如都用文本文件，逐条存储，并带有表头，用于匹配Variables名。
+ 站点泰森多边形此处并没有什么用处，只是读取了站点信息，应当将站点信息存入文本文件，而将泰森多边形作为[Spatial Data](#3-spatial-data)的输入信息

## 2 Hydrological Data
Include **Discharge** daily and storm date.
~~~
ImportDailyDischargeData(hostname,port,climateDBName,DischargeExcelPrefix,DischargeYear)
~~~
**TODO**: 
+ 流量日值数据的导入和气象数据存在问题一样，需组织成二维矩阵形式的excel表格，非常不方便。
+ 现在没将暴雨流量数据导入放进来，之后也需要对其进行统一格式整理
+ 我觉得最好是将流量测量的原始数据存入数据库，用的时候再读取出来，处理后使用

## 3 Spatial Data
