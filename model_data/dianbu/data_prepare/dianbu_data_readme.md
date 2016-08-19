## 店埠河小流域SEIMS模型准备数据

### 1 气候数据

#### 1.1 数据来源

##### 1.1.1 降水数据
降水数据来源于[安徽省水文遥测信息网](http://yc.wswj.net/ahyc/ "安徽省水文遥测信息网")，利用Python实现自动下载功能，[点此查看源代码](https://github.com/crazyzlj/Python/blob/master/HydroDataDownload/anhui_precipitation_download.py "python-download-anhui-rainfall")。

研究区周围共有5个站点，分别是：管湾'62915310', 胡岗'62942747', 众兴'62903180', 群治'62942717', 肥东'62922800'，数据时间为2013-2015年，分辨率为1天。

##### 1.1.2 气象数据
由于研究区周围仅有一个国家气象站点，合肥站（58321），因此采用这一个站点的日值观测数据。数据来源为[中国气象数据网](http://data.cma.cn/ "中国气象数据网")的中国地面气候资料日值数据集(V3.0)产品，[点此查看数据下载方法](http://zhulj.net/python/2016/04/11/Constructing-SURF_CLI_CHN_MUL_DAY_V3.0-database.html)。

2015年12月资料没有更新。暂时用2014年12月的替代。

对于日尺度模型的构建，所需气象数据如下：
+ 气象数据(**日值**)
	+ StationID: 必需，站点编号(整型)
	+ Y, M, D  : 必需，年/月/日
	+ TMAX     : 必需，最高气温(deg C)
	+ TMIN     : 必需，最低气温(deg C)
	+ TMEAN    : 可选，平均气温(deg C)，缺失情况下，由(TMAX+TMIN)/2计算
	+ RM       : 必需，相对湿度(%)
	+ WS       : 必需，风速(m/s)
	+ SR/SSD   : 二选一，太阳辐射(MJ/m2/day)/日照时数(h)
	+ PET      : 可选，蒸散发(mm)

##### 1.2 数据整理

+ 降水站点信息文件
	+ `/home/zhulj/models/dianbu/data_prepare/climate/Sites_P.txt`
	+ 站点信息包括 站点编号(整数型)、站点名称(字符串)、投影系X坐标及Y坐标、经纬度、以及高程，如下表所示，站点信息保存为txt格式，如:

|StationID|Name|LocalX|LocalY|Lon|Lat|Elevation|
|----|----|----|----|----|----|----|
|62915310|GuanWan|39548201.84|3543440.687|117.525278|32.025556|43|
|...|...|...|...|...|...|...|

+ 气象站点信息文件

	+ 格式与降水站点一致，`/home/zhulj/models/dianbu/data_prepare/climate/Sites_M.txt`

|StationID|Name|LocalX|LocalY|Lon|Lat|Elevation|
|----|----|----|----|----|----|----|
|58321|hefei|39535576.056|3536052.03009|117.23|31.87|20|

+ 降水日值数据文件
	+ 降水数据采用文本文件组织，基本格式如下：
	+ `/home/zhulj/models/dianbu/data_prepare/climate/precipitation_dianbu_daily.txt`
	
|Y|M|D|StationID1|StationID2|...|
|----|----|----|----|----|----|
|2014|1|1|0|0.5|...|

+ 气象日值数据文件
	+ 气象数据采用文本文件组织，基本格式如下：
	+ 没有数据的属性列不写，但是必需属性需准备
	+ `/home/zhulj/models/dianbu/data_prepare/climate/meteorology_dianbu_daily.txt`

|StationID|Y|M|D|TMEAN|TMAX|TMIN|RM|WS|SSD|...|
|----|----|----|----|----|----|----|----|----|----|----|
|58321|2010|1|1|3.5|8.6|-0.8|69|1.9|4.7|...|


### 2 空间数据
#### 2.1 土壤
+ (Deprecated)soil_SEQN_bsn1_old_10m.tif、soil_SEQN_bsn1_old_30m.tif为1号小流域，安徽1:100万土壤图
+ soil_SEQN_10m.tif、soil_SEQN_10m.tif为1、2、3号小流域，肥东土肥站绘制土壤图
####2.2 土地利用
+ landuse_10m.tif、landuse_30m.tif为1、2、3号小流域
+ landuse_bsn1_10m.tif、landuse_bsn1_30m.tif为1号小流域


### 建模过程

#### .1 子流域划分

设置`mgtFieldFile = None（line 42 in *.ini file）`，并利用命令`python G:\code_zhulj\SEIMS\preprocess\subbasin_delineation.py -ini G:\code_zhulj\SEIMS\preprocess\dianbu2_30m_longterm_omp_zhulj_winserver.ini`进行子流域划分，结合DEM分析，最终确定一个合适的汇流累积量阈值，即`D8AccThreshold（line 47 in *.ini file）`。

#### .2 管理单元划分

利用流向、土地利用、子流域等进行管理单元划分，目前可以利用吴辉博士开发的考虑上下游关系地块划分方法，即预处理程序中的`fieldpartition`。


