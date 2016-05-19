TODO LISTS
=======

# 1.代码注释工作

## 1.1已完成模块

+ Base模块组下面的Util（遗留MetadataInfo没有注释）、data、

+ Modules 模块组
	+ Climate 模块组 


# 2.代码整理工作

## 2.1Data模块

弃用列表

+ SQLite相关
	+ clsHydroClimateData


# 3.功能补充

## 3.1模型结构

+ 1. 土壤层数问题

目前是2层，程序中有些地方明确写死为2层，比如`MongoUtil.cpp`里的`Read2DSoilAttr`函数，这样不够合理，应改为可支持多层土壤层数。

+ 2.




======
# TODO by Gao
##1.代码整理
+ 1 nReach等变量赋值问题
	+ 未直接赋值，通过相关变量间接赋值，
	+ 模块：Hydrology_longterm - IKW_REACH
	+ 模块：Hydrology_longterm - IUH_IF
	+ 模块：Hydrology_longterm - IUH_OF
+ 2 DT_Array2D变量及所在模块

模块|变量名
---|---
GWA_RE|VAR_PERCO
GWA_RE|VAR_SOMO
GWA_RE|VAR_GWWB
	
+ 3 变量定义不明确问题
	+ Hydrology_longterm - IKW_REACH: VAR_QOUTLET & VAR_QSOUTLET
	+ Hydrology_longterm - IKW_REACH: VAR_MSF & MUSK_CH: VAR_VSF

+ 4 add*()与set*()、get*()中变量不一致的问题
	+ Hydrology_longterm - IKW_REACH: VAR_QUPREACH










	
	