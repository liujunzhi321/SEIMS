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

+ 1. 土壤数据问题

目前土壤数据的的输入为机械组成和有机质含量的TIFF，这样做可扩展性太差。而且，土层默认为2层，程序中有些地方明确写死为2层，比如`MongoUtil.cpp`里的`Read2DSoilAttr`函数，这样不够合理。应改为可支持多层土壤层数。

+ 2.

> 2016-5-18 已完成 `DT_Rater2D`的读写，详见`clsRaterData`类
> 2016-5-21 已完成多层土壤理化性质的计算，但是，对于设置层数大于3，但是没有对应层数属性的情况，暂未考虑，SWAT中有计算，所以以后考虑加上！
> 

+ 2. 河道数据结构

目前有两个河道相关的AddParameter，即Tag_ReachParameter和Tag_RchParam，但是读取的均是reaches表。因此，有必要进行合并，用一个clsReach类替代。



## TODO by Zhu

### KINWAVSED_CH模块

+ 1. Tag_ReachParameter问题

### KINWAVSED_OL模块

+ 1. mdi.AddInput("QRECH", "m3/s", "Flux in the downslope boundary of cells", "Module",DT_Array2D); QRECH变量名与MUSK_CH中河道出口流量冲突，而且DimensionType也不相符
+ 


### MUSLE_AS模块

+ 1. SEDTOCH和SEDTOCH_T单位到底是吨还是千克？

## TODO by Gao

##1.代码整理
+ 1 nReach等变量赋值问题
	+ 未直接赋值，通过相关变量间接赋值，
	+ 模块：Hydrology_longterm - IKW_REACH
	+ 模块：Hydrology_longterm - IUH_IF
	+ 模块：Hydrology_longterm - IUH_OF
	
+ 2 DT_Array2D变量及所在模块

模块|子模块|变量名|类型
---|---|---|---
Hydrology_longterm|GWA_RE|VAR_PERCO|AddInput
Hydrology_longterm|GWA_RE|VAR_SOMO|AddInput
Hydrology_longterm|PEI_PI|VAR_SOMO|AddInput
Hydrology_longterm|PEI_STR|VAR_SOMO|AddInput
Hydrology_longterm|SET_LM|VAR_SOMO|AddInput
Hydrology_longterm|SOL_WB|VAR_PERCO|AddInput
Hydrology_longterm|SOL_WB|VAR_SOMO|AddInput
Hydrology_longterm|SOL_WB|VAR_SSRU|AddInput
Hydrology_longterm|SSR_DA|VAR_SOMO|AddInput
Hydrology_longterm|SUR_CN|VAR_SOMO|AddOutput

	
+ 3 变量定义不明确问题
	+ Hydrology_longterm - IKW_REACH: VAR_QOUTLET & VAR_QSOUTLET
	+ Hydrology_longterm - IKW_REACH: VAR_MSF & MUSK_CH: VAR_VSF

+ 4 add-()与set-()、get-()中变量不一致的问题
	+ Hydrology_longterm - IKW_REACH: VAR_QUPREACH
	+ Hydrology_longterm - SSR_DA: Tag_FLOWIN_PERCENTAGE_D8
	
+ 5 变量单位不明确问题
	+ Hydrology_longterm - SON_DD: VAR_C_SNOW
	+ Hydrology_longterm - SON_DD: VAR_C_RAIN
	+ Hydrology_longterm - SON_SP: VAR_C_SNOW6 ("mm/oC/day")
	+ Hydrology_longterm - SON_SP: VAR_C_SNOW12 ("mm/oC/day")












