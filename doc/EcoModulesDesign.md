生态模块设计
-----

SWAT模型采用了简化的EPIC模型，SEIMS将其借鉴并重新组织。

在《SWAT Theory 2009》中关于生态（植被生长）部分主要为第5部分——Land Cover/Plant，和第6部分的第1章节——General Management，下面根据该书的章节顺序，整理了对应的源码文件：

+ Section 5: Land Cover/Plant

	+ 5:1 Growth Cycle

	+ 5:2 Optimal Growth

	```
	1. plantmod.f: 计算总植物生物量和根系生物量的日尺度潜在生长量，计算LAI
	2. swu.f: 计算植被潜在蒸发量在根系中的分布、基于土壤可获得水量计算植被实际用水量，并估计水胁迫因子
	```

	+ 5:3 Actual Growth

	```
	3. grow.f: 考虑水、温度、养分对植被的胁迫作用，校正植被生物量、LAI、冠层高度的计算
	其中，调用tstr.f计算温度胁迫，调用nup.f计算氮吸收（如果作物为豆科作物，调用nfix.f计算氮固定，调用nuts.f计算氮磷胁迫），调用npup.f计算磷吸收，调用氮元素自动施肥
	```

	+ 5:4 Crop Yield
	
	+ SEIMS 模块计划
	
		+ 植被潜在蒸腾（Priestley-Taylor 和 Hargreaves 蒸散发计算方法）、潜在/实际土壤蒸发量。目前SEIMS模型中有`PET_PT, PET_H, PET_PM`,需要看是直接将植被蒸腾计算添加进现有模块还是重新设置模块。 涉及到的SWAT源码为`etpot.f, etact.f` --- **计划 2016-4-21 完成**
		+ 

+ Section 6: Management Practices

	+ 6:1 General Management
	
		```
		operatn.f, sched_mgt.f (包括plantop.f, dormant.f, harvkillop.f, harvestop.f, killip.f, tillmix.f, graze.f等)`
		施肥相关 fert.f, confert.f
		
		```

		SWAT中，每个HRU对应一个`.mgt`文件，定义管理措施，对应于Access数据库中，Management Scheduled Operations对应于`mgt2`表，而常规管理措施对应于`mgt1`表。

		SEIMS沿用一张表设置管理措施的方式。但是输入数据的准备比较麻烦，目前SWAT Editor设置管理措施也很不方便。

		> 需要设计一种简便的方法设置每种土地利用的管理措施（或者加上其他限制条件）。

		> TODO: ManagementType.txt中新加了一个`FIELDS`列，用于整理每种管理措施需要填写的属性列表
