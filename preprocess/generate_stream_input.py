from osgeo import gdal
from osgeo import ogr
import networkx as nx
import pylab
import math
import sys
import os
import pymongo
from pymongo import Connection
from pymongo.errors import ConnectionFailure
import shutil
from adjust_groups import *
from config import *

sys.setrecursionlimit(10000)

def gridNumber(watershedFile):
    numDic = {}
    ds = gdal.Open(watershedFile)
    band = ds.GetRasterBand(1)
    data = band.ReadAsArray()
    xsize = band.XSize
    ysize = band.YSize
    noDataValue = band.GetNoDataValue()
    for i in range(ysize):
        for j in range(xsize):
            k = int(data[i][j])
            if(abs(k - noDataValue) > 0.00001):
                numDic[k] = numDic.setdefault(k,0) + 1
    return numDic, ds.GetGeoTransform()[1]
    
def DownstreamUpOrder(orderDic, g, node, orderNum):
    orderDic[node] = orderNum
    for inNode in g.in_edges(node):
        DownstreamUpOrder(orderDic, g, inNode[0], orderNum+1)
    
def downStream(reachFile):
    downStreamDic = {}
    depthDic = {}
    slopeDic = {}
    widthDic = {}
    lenDic = {}
    dsReach = ogr.Open(reachFile)
    layerReach = dsReach.GetLayer(0)
    layerDef = layerReach.GetLayerDefn()
    iFrom = layerDef.GetFieldIndex("LINKNO") # TauDEM: LINKNO; ArcSWAT: FROM_NODE
    iTo = layerDef.GetFieldIndex("DSLINKNO") # TauDEM: DSLINKNO; ArcSWAT: TO_NODE
    iDepth = layerDef.GetFieldIndex("Depth")
    iSlope = layerDef.GetFieldIndex("Slope") # TauDEM: Slope (tan); ArcSWAT: Slo2 (100*tan)
    iWidth = layerDef.GetFieldIndex("Width") 
    iLen = layerDef.GetFieldIndex("Length") # TauDEM: Length; ArcSWAT: Len2
    
    g = nx.DiGraph()
    ft = layerReach.GetNextFeature()
    while ft is not None:
        nodeFrom = ft.GetFieldAsInteger(iFrom)
        nodeTo = ft.GetFieldAsInteger(iTo)
        if iDepth > -1:
            depthDic[nodeFrom] = ft.GetFieldAsDouble(iDepth)
        else:
            depthDic[nodeFrom] = 1
            
        if iDepth > -1:
            slopeDic[nodeFrom] = ft.GetFieldAsDouble(iSlope)
            if slopeDic[nodeFrom] < 0.0001:
                slopeDic[nodeFrom] = 0.0001
        else:
            slopeDic[nodeFrom] = 0.0001
            
        if iWidth > -1:
            widthDic[nodeFrom] = ft.GetFieldAsDouble(iWidth)
        else:
            widthDic[nodeFrom] = 10
            
        lenDic[nodeFrom] = ft.GetFieldAsDouble(iLen)
        downStreamDic[nodeFrom] = nodeTo
        if nodeTo > 0:
            #print nodeFrom, nodeTo
            g.add_edge(nodeFrom, nodeTo)
        ft = layerReach.GetNextFeature()
        
    # find outlet subbasin
    for node in g.nodes():
        if g.out_degree(node) == 0:
            outlet = node
    print 'outlet subbasin:', outlet
    
    # assign order from outlet to upstream subbasins   
    downstreamUpOrderDic = {}
    DownstreamUpOrder(downstreamUpOrderDic, g, outlet, 1)
    #find the maximum order nubmer
    maxOrder = 0
    for k in downstreamUpOrderDic.keys():
        if(downstreamUpOrderDic[k] > maxOrder):
            maxOrder = downstreamUpOrderDic[k]
    #reserve the order number
    for k in downstreamUpOrderDic.keys():
        downstreamUpOrderDic[k] = maxOrder - downstreamUpOrderDic[k] + 1
        
    # assign order from the source subbasins
    upstreamDownOrderDic = {}
    orderNum = 1
    nodelist = g.nodes()
    while(len(nodelist) != 0):
        nodelist = g.nodes()
        delList = []
        for node in nodelist:
            if(g.in_degree(node) == 0):
                upstreamDownOrderDic[node] = orderNum
                delList.append(node)
        for item in delList:
            g.remove_node(item)
        orderNum = orderNum + 1

    return downStreamDic, downstreamUpOrderDic, upstreamDownOrderDic,\
           depthDic, slopeDic, widthDic, lenDic

def add_group_field(shpFile, subbasinFieldName, n, groupKmetis, groupPmetis, ns):
        dsReach = ogr.Open(shpFile, update = True)
        layerReach = dsReach.GetLayer(0)
        layerDef = layerReach.GetLayerDefn()
        iCode = layerDef.GetFieldIndex(subbasinFieldName)
        iGroup = layerDef.GetFieldIndex("GROUP")
        iGroupPmetis = layerDef.GetFieldIndex("GROUP_P")
        if(iGroup < 0):
            new_field = ogr.FieldDefn("GROUP", ogr.OFTInteger)
            layerReach.CreateField(new_field)
        if(iGroupPmetis < 0):
            new_field = ogr.FieldDefn("GROUP_P", ogr.OFTInteger)
            layerReach.CreateField(new_field)            
        #grid_code:feature map
        ftmap = {}
        layerReach.ResetReading()
        ft = layerReach.GetNextFeature()
        while ft is not None:
            id = ft.GetFieldAsInteger(iCode)
            ftmap[id] = ft
            ft = layerReach.GetNextFeature()
            
        groupDic = {}
        groupDicPmetis = {}        
        i = 0
        for node in ns:
            groupDic[node] = groupKmetis[i]
            groupDicPmetis[node] = groupPmetis[i]
            ftmap[node].SetField("GROUP", groupKmetis[i])
            ftmap[node].SetField("GROUP_P", groupPmetis[i])
            layerReach.SetFeature(ftmap[node])
            i = i + 1
        
        layerReach.SyncToDisk()
        dsReach.Destroy()
        del dsReach
        
        #copy the reach file to new file
        extlist = ['.shp', '.dbf', '.shx']
        for ext in extlist:
            prefix = os.path.splitext(shpFile)[0]
            src = prefix + ext
            dst = prefix + "_" + str(n) + ext
            if os.path.exists(dst):
                os.remove(dst)
            shutil.copy(src, dst)
            
        return groupDic, groupDicPmetis
            
def GenerateReachTable(folder, db, forCluster):
    watershedFile = folder + os.sep + subbasinOut
    reachFile = folder + os.sep + "reaches" + os.sep + reachesOut
    subbasinFile = folder + os.sep + "subbasins" + os.sep + subbasinVec
    #print reachFile
    
    areaDic, dx = gridNumber(watershedFile)
    downStreamDic, downstreamUpOrderDic, upstreamDownOrderDic, \
         depthDic, slopeDic, widthDic, lenDic = downStream(reachFile)
    #for k in downStreamDic:
    #    print k, downStreamDic[k]
    
    g = nx.DiGraph()
    for k in downStreamDic:
        if(downStreamDic[k] > 0):
            g.add_edge(k, downStreamDic[k])
            
    ns = g.nodes()
    
    #consturct the METIS input file
    metisFolder = folder + os.sep + "metis_output"
    if not os.path.exists(metisFolder):
        os.mkdir(metisFolder)
    metisInput = r'%s/metis.txt' % (metisFolder)
    f = open(metisInput, 'w')
    f.write(str(len(ns)) + "\t" + str(len(g.edges())) + "\t" + "010\t1\n")
    for node in ns:
        if node <= 0:
            continue
        f.write(str(areaDic[node]) + "\t")
        for e in g.out_edges(node):
            if e[1] > 0:
                f.write(str(e[1]) + "\t")
        for e in g.in_edges(node):
            if e[0] > 0:
                f.write(str(e[0]) + "\t")
        f.write("\n")
    f.close()
    
    #execute metis
    nlist = [1,]
    if(forCluster):
        a = [1,2,3,6]
        a2 = [12*pow(2,i) for i in range(8)]
        a.extend(a2)

        b = [1,3]
        b2 = [i/2 for i in a2]
        b.extend(b2)

        c = [1,2]
        c2 = [i/3 for i in a2]
        c.extend(c2)

        d = [1]
        d2 = [i/6 for i in a2]
        d.extend(d2)

        e = [i/12 for i in a2]

        nlist = a+b+c+d+e
        nlist.extend(range(1,129))
        #nlist.extend([576, 288, 512, 258, 172])
        nlist = list(set(nlist))
        nlist.sort()

        
    #interpolation among different stream orders
    minManning = 0.035
    maxManning = 0.075
    
    minOrder = 1
    maxOrder = 1
    for k in upstreamDownOrderDic.keys():
        if(upstreamDownOrderDic[k] > maxOrder):
            maxOrder = upstreamDownOrderDic[k]
    
    dicManning = {}
    a = (maxManning-minManning) / (maxOrder-minOrder)
    for id in downStreamDic.keys():
        dicManning[id] = maxManning - a * (upstreamDownOrderDic[id] - minOrder)
            
    for n in nlist:
        #print 'divide number: ', n
        if(n == 1):
            for id in downStreamDic:
                dic = {}
                dic['SUBBASIN'] = id
                dic['DOWNSTREAM'] = downStreamDic[id]
                dic['UP_DOWN_ORDER'] = upstreamDownOrderDic[id]
                dic['DOWN_UP_ORDER'] = downstreamUpOrderDic[id]
                dic['MANNING'] = dicManning[id]
                dic['SLOPE'] = slopeDic[id]
                dic['V0'] = math.sqrt(slopeDic[id]) * math.pow(depthDic[id], 2./3) / dic['MANNING']
                dic['NUM_CELLS'] = areaDic[id]
                dic['GROUP'] = 1
                dic['GROUP_DIVIDE'] = 1
                dic['WIDTH'] = widthDic[id]
                dic['LENGTH'] = lenDic[id] 
                dic['DEPTH'] = depthDic[id]   
                dic['AREA'] = areaDic[id]*dx*dx     
                db.reaches.insert(dic, safe=True)
            continue
        
        #kmetis
        strCommand = '%s/gpmetis %s %d' % (METIS_DIR,metisInput, n)
        result = os.popen(strCommand)
        fMetisOutput = open('%s/kmetisResult%d.txt' % (metisFolder, n), 'w')
        for line in result.readlines():
            fMetisOutput.write(line)
        fMetisOutput.close()
        
        metisOutput = "%s.part.%d" % (metisInput, n)
        f = open(metisOutput)
        lines = f.readlines()
        groupKmetis = [int(item) for item in lines]
        f.close()
        AdjustGroupResult(g, areaDic, groupKmetis, n)
        
        #pmetis
        strCommand = '%s/gpmetis -ptype=rb %s %d' % (METIS_DIR,metisInput, n)
        result = os.popen(strCommand)
        fMetisOutput = open('%s/pmetisResult%d.txt' % (metisFolder, n), 'w')
        for line in result.readlines():
            fMetisOutput.write(line)
        fMetisOutput.close()
        
        f = open(metisOutput)
        lines = f.readlines()
        groupPmetis = [int(item) for item in lines]
        f.close()
        AdjustGroupResult(g, areaDic, groupPmetis, n)

        groupDicK, groupDicP = add_group_field(reachFile, "LINKNO", n, groupKmetis, groupPmetis, ns)
        groupDicK, groupDicP = add_group_field(subbasinFile, "Subbasin", n, groupKmetis, groupPmetis, ns)
        
        #get database
        for id in downStreamDic:
            dic = {}
            dic['SUBBASIN'] = id
            dic['DOWNSTREAM'] = downStreamDic[id]
            dic['UP_DOWN_ORDER'] = upstreamDownOrderDic[id]
            dic['DOWN_UP_ORDER'] = downstreamUpOrderDic[id]
            dic['MANNING'] = dicManning[id]
            dic['V0'] = math.sqrt(slopeDic[id]) * math.pow(depthDic[id], 2./3) / dic['MANNING']
            dic['NUM_CELLS'] = areaDic[id]
            dic['GROUP_KMETIS'] = groupDicK[id]
            dic['GROUP_PMETIS'] =  groupDicP[id]
            dic['GROUP_DIVIDE'] = n
            dic['WIDTH'] = widthDic[id]
            dic['LENGTH'] = lenDic[id] 
            dic['DEPTH'] = depthDic[id]  
            dic['AREA'] = areaDic[id]*dx*dx                
            db.reaches.insert(dic, safe=True)       
         
    db.reaches.create_index("GROUP_DIVIDE")
    db.reaches.create_index([('SUBBASIN', pymongo.ASCENDING), ('GROUP_DIVIDE', pymongo.ASCENDING)])
    
    print 'The reach table is generated!'

if __name__ == "__main__":
    conn = Connection(host='192.168.6.55', port=27017)
    workingDir = '/data/liujz/data/fenkeng/output'
    db = conn['cluster_model_fenkeng_90m']
    GenerateReachTable(workingDir, db, True)
