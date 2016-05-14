### This file is used for TODO lists

#TODO: 沈芳：数据预处理这里面每一段代码你都需要理解透彻，找到原理依据，明白为什么这么做。并找出不合理的地方，改进！
#TODO: 现在的原则是，使这套预处理程序尽可能灵活、可扩展性强，关于气象数据导入的程序，我修改过了，你可以学习一下
#TODO: 原则就是：凡事字符串均在text.py中进行定义，以满足将来修改的需要，务必每个文件都检查。

#比如：
#TODO: chwidth.py Figure out what's a and b means and how to determine them, and/or move it to text.py or config.py? LJ

#TODO: 目前读取SQLite数据库均是在每个py内，能否将这些操作单独拎出来？参考https://github.com/crazyzlj/Python/blob/master/HydroDataDownload/CreateDatabase_SURF_CLI_CHN_MUL_DAY.py

#TODO: 另外，一定安装一款易用的python IDE比如pycharm，方便调试、替换变量名等！

#TODO: 对于程序灵活性，举个例子讲：PrecipitationDaily.py中，ImportSites(db, shpFileName, siteType)函数要求站点的Shapefile属性表
#TODO: 必须按照指定的字符串定义，但是我们可以用查找关键字，比如Lat, lati, LAT等都可以认为是纬度。 对于大小写字符串对比，
#TODO: 我在util.py里定义了StringMatch()函数。

#TODO: 现有的crop.txt替换成CropLookup.txt，相关文件有reclass_crop.py等。需要细致修改。

#TODO: 多学习，多思考。 目前代码中不完美的地方还有很多，发现并改进！


