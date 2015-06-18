#! /usr/bin/env python
#coding=utf-8
## @TauDEM Functions
# 
#  includes Fill, FlowDirD8, FlowDirDinf, FlowAccD8, StreamRaster, MoveOutlet, 
#           StreamSkeleton(peukerdouglas), StreamNet, DropAnalysis
#   

import os, platform
import subprocess

def Fill(np, workingDir, dem, filledDem, mpiexeDir = None,exeDir=None):
    os.chdir(workingDir)
    if exeDir is not None:
        exe = exeDir + os.sep + "pitremove"
    else:
        exe = "pitremove"
    strCmd = "mpiexec -n %d %s -z %s -fel %s" % (np, exe, dem, filledDem)
    if mpiexeDir is not None:
        strCmd = mpiexeDir + os.sep + strCmd
    print strCmd
    process=subprocess.Popen(strCmd, shell=True, stdout=subprocess.PIPE)
    return process.stdout.readlines()

def FlowDirD8(np, workingDir, filledDem, flowDir, slope, mpiexeDir = None, exeDir=None):
    os.chdir(workingDir)
    if exeDir is not None:
        exe = exeDir + os.sep + "d8flowdir"
    else:
        exe = "d8flowdir"
    strCmd = "mpiexec -n %d %s -fel %s -p %s  -sd8 %s" % (np,exe, filledDem, flowDir, slope)
    if mpiexeDir is not None:
        strCmd = mpiexeDir + os.sep + strCmd    
    print strCmd
    process=subprocess.Popen(strCmd, shell=True, stdout=subprocess.PIPE)
    return process.stdout.readlines()
    

def FlowDirDinf(np, workingDir, filledDem, flowAngle, slope, mpiexeDir = None,exeDir=None):
    os.chdir(workingDir)
    if exeDir is not None:
        exe = exeDir + os.sep + "dinfflowdir"
    else:
        exe = "dinfflowdir"
    strCmd = "mpiexec -n %d %s -fel %s -ang %s -slp %s" % (np, exe, filledDem, flowAngle, slope)
    if mpiexeDir is not None:
        strCmd = mpiexeDir + os.sep + strCmd
    print strCmd
    process=subprocess.Popen(strCmd, shell=True, stdout=subprocess.PIPE)
    return process.stdout.readlines()

def FlowAccD8(np, workingDir, flowDir, acc, outlet=None, streamSkeleton=None, mpiexeDir = None,exeDir=None):
    os.chdir(workingDir)
    if exeDir is not None:
        exe = exeDir + os.sep + "aread8"
    else:
        exe = "aread8"
    if outlet is not None:
        if streamSkeleton is not None:
            strCmd = "mpiexec -n %d %s -p %s -o %s -wg %s -ad8 %s -nc" % (np,exe, flowDir, outlet, streamSkeleton, acc)
        else:
            strCmd = "mpiexec -n %d %s -p %s -o %s -ad8 %s -nc" % (np,exe, flowDir, outlet, acc)
    else:
        if streamSkeleton is not None:
            strCmd = "mpiexec -n %d %s -p %s -wg %s -ad8 %s -nc" % (np,exe, flowDir, streamSkeleton, acc)
        else:
            strCmd = "mpiexec -n %d %s -p %s -ad8 %s -nc" % (np,exe, flowDir, acc)
    # -nc means donot consider edge contaimination
    if mpiexeDir is not None:
        strCmd = mpiexeDir + os.sep + strCmd
    print strCmd
    process=subprocess.Popen(strCmd, shell=True, stdout=subprocess.PIPE)
    return process.stdout.readlines()

def StreamRaster(np, workingDir, acc, streamRaster, threshold=1000, mpiexeDir = None,exeDir=None):
    os.chdir(workingDir)
    if exeDir is not None:
        exe = exeDir + os.sep + "threshold"
    else:
        exe = "threshold"
    strCmd = "mpiexec -n %d %s -ssa %s -thresh %s  -src %s" % (np, exe,acc, str(threshold), streamRaster)
    if mpiexeDir is not None:
        strCmd = mpiexeDir + os.sep + strCmd
    print strCmd
    process=subprocess.Popen(strCmd, shell=True, stdout=subprocess.PIPE)
    return process.stdout.readlines()

def MoveOutlet(np, workingDir, flowDir, streamRaster, outlet, modifiedOutlet, mpiexeDir = None,exeDir=None):
    os.chdir(workingDir)
    if exeDir is not None:
        exe = exeDir + os.sep + "moveoutletstostreams"
    else:
        exe = "moveoutletstostreams"
    strCmd = "mpiexec -n %d %s -p %s -src %s -o %s -om %s" % (np, exe,flowDir, streamRaster, outlet, modifiedOutlet)
    if mpiexeDir is not None:
        strCmd = mpiexeDir + os.sep + strCmd
    print strCmd
    process=subprocess.Popen(strCmd, shell=True, stdout=subprocess.PIPE)
    return process.stdout.readlines()

def StreamSkeleton(np, workingDir, filledDem, streamSkeleton, mpiexeDir = None,exeDir=None):
    os.chdir(workingDir)
    if exeDir is not None:
        exe = exeDir + os.sep + "peukerdouglas"
    else:
        exe = "peukerdouglas"
    strCmd = "mpiexec -n %d %s -fel %s -ss %s" % (np, exe,filledDem, streamSkeleton)
    if mpiexeDir is not None:
        strCmd = mpiexeDir + os.sep + strCmd
    print strCmd
    process=subprocess.Popen(strCmd, shell=True, stdout=subprocess.PIPE)
    return process.stdout.readlines()

def StreamNet(np, workingDir, filledDem, flowDir, acc, streamRaster, modifiedOutlet, streamOrder, chNetwork, chCoord, streamNet, subbasin, mpiexeDir = None,exeDir=None):
    os.chdir(workingDir)
    if exeDir is not None:
        exe = exeDir + os.sep + "streamnet"
    else:
        exe = "streamnet"
    strCmd = "mpiexec -n %d %s -fel %s -p %s -ad8 %s -src %s -o %s  -ord %s -tree %s -coord %s -net %s -w %s" % (np,exe, filledDem, flowDir, acc, streamRaster, modifiedOutlet, streamOrder, chNetwork, chCoord, streamNet, subbasin)
    if mpiexeDir is not None:
        strCmd = mpiexeDir + os.sep + strCmd        
    print strCmd
    process=subprocess.Popen(strCmd, shell=True, stdout=subprocess.PIPE)
    return process.stdout.readlines()
def DropAnalysis(np,workingDir,fel,p,ad8,ssa,shapefile,minthresh,maxthresh,numthresh,logspace,drp, mpiexeDir = None, exeDir=None):
    os.chdir(workingDir)
    if exeDir is not None:
        exe = exeDir + os.sep + "dropanalysis"
    else:
        exe = "dropanalysis"
    strCmd = "mpiexec -n %d %s -fel %s -p %s -ad8 %s -ssa %s -o %s -drp %s -par %f %f %f" % (np,exe,fel,p,ad8,ssa,shapefile,drp,minthresh,maxthresh,numthresh)
    if logspace == 'false':    
        strCmd = strCmd + ' 1'
    else:
        strCmd = strCmd + ' 0'
    if mpiexeDir is not None:
        strCmd = mpiexeDir + os.sep + strCmd
    print strCmd
    process=subprocess.Popen(strCmd, shell=True, stdout=subprocess.PIPE)
    return process.stdout.readlines()