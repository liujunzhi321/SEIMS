# Manual for Developers in Chinese v1.0
==========

prepared by Liangjun Zhu

----------

# 目录
[**前言**](#前言)

[**Github使用说明**](#Github使用说明)

1. [Windows下安装Git](#Windows下安装Git)

2. [从Github中克隆代码库](#从Github中克隆代码库)

3. [代码同步与更新](#代码同步与更新)

[**SEIMS源码编译**](#SEIMS源码编译)

1. [源码结构](#源码结构)

2. [Windows下VS编译](#Windows下VS编译)

3. [Linux下GNU编译](#Linux下GNU编译)

[**运行SEIMS**](#运行SEIMS)

1. [mongoDB](#mongoDB)

2. [模型配置文件](#模型配置文件)

3. [运行命令](#运行命令)

[**SEIMS基本框架**](#SEIMS基本框架)

[**SEIMS模块编写流程**](#SEIMS模块编写流程)

[**编码规范**](#编码规范)

1. [doxygen注释规则](#doxygen注释规则)

2. [Markdown文件编辑](#Markdown文件编辑)


# 前言
-----------
SEIMS（Spatially Explicit Integrated Modeling System）模型是由刘军志等人开发的以栅格为基本模拟单元、能体现水流空间运动的全分布式水文模型。SEIMS在借鉴Wetspa、LISEM、CASC2D、DHSVM和流溪河模型等多个模型基础上开发的模块化水文模型，采用C++编写，实现了子流域-基本单元的双层并行，采用的并行策略为共享内存（OpenMP）和消息传递（MPI），并利用NoSql数据库MongoDB进行数据组织管理。模型可在Windows及Linux平台下运行。

目前，模型还不完善，主要存在以下主要问题：

1. 生态和养分循环模块还很初步，距实用还相差很远；
2. 水文和侵蚀模块虽相对成熟，但尚缺乏进一步的验证工作。

因此需要大家群策群力完善模型，主要有以下几个目标：

1. 每个模块都具有完善的理论和开发文档；
2. 编码规范（包括模块和变量命名，编程习惯等），空闲了可以多看看Google C++ Code Style（[EN](http://google-styleguide.googlecode.com/svn/trunk/cppguide.html)，[CN](http://www.cnblogs.com/ggjucheng/archive/2012/01/02/2310278.html)）；
3. 编写模块需要与当前主流的其他模型进行对比验证，多做测试；
4. 为用户提供易用的Web界面，用户可选择在服务器端运行模型，也可以选择把模型和数据下载到本机运行。

# Github使用说明
为了便于模型开发的多人协作，计划采用Github平台进行版本控制。关于Git的详细教程，推荐阅读[廖学峰的网站](http://www.liaoxuefeng.com/wiki/0013739516305929606dd18361248578c67b8067c8c017b000/)这一节主要介绍我们需要用到的一些命令操作。
[返回目录](#目录)
## Windows下安装Git

[返回目录](#目录)
## 从Github中克隆代码库
[返回目录](#目录)
## 代码同步与更新
[返回目录](#目录)

# SEIMS源码编译
[返回目录](#目录)
## 源码结构
[返回目录](#目录)
## Windows下VS编译
[返回目录](#目录)
## Linux下GNU编译
[返回目录](#目录)
# 运行SEIMS
[返回目录](#目录)
## mongoDB
[返回目录](#目录)
## 模型配置文件
[返回目录](#目录)
## 运行命令
[返回目录](#目录)
# SEIMS基本框架
[返回目录](#目录)
# SEIMS模块编写流程
[返回目录](#目录)
# 编码规范
[返回目录](#目录)
## doxygen注释规则
[返回目录](#目录)
## Markdown文件编辑
[返回目录](#目录)