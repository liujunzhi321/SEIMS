from osgeo import gdal
from osgeo import ogr
import networkx as nx
import math
import pymongo
from pymongo import MongoClient
import shutil
from adjust_groups import *
from text import *
from util import *

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
            if(abs(k - noDataValue) > DELTA):
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
            if slopeDic[nodeFrom] < MINI_SLOPE:
                slopeDic[nodeFrom] = MINI_SLOPE
        else:
            slopeDic[nodeFrom] = MINI_SLOPE
            
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
    reachFile = folder + os.sep + DIR_REACHES + os.sep + reachesOut
    subbasinFile = folder + os.sep + DIR_SUBBASIN + os.sep + subbasinVec
    #print reachFile
    
    areaDic, dx = gridNumber(watershedFile)
    downStreamDic, downstreamUpOrderDic, upstreamDownOrderDic, \
         depthDic, slopeDic, widthDic, lenDic = downStream(reachFile)
    #for k in downStreamDic:
        #print k, downStreamDic[k]
    
    g = nx.DiGraph()
    for k in downStreamDic:
        if(downStreamDic[k] > 0):
            g.add_edge(k, downStreamDic[k])
            
    ns = g.nodes()
    
    #consturct the METIS input file
    metisFolder = folder + os.sep + DIR_METIS
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
        print 'divide number: ', n
        if(n == 1):
            for id in downStreamDic:
                dic = {}
                dic[REACH_SUBBASIN] = id
                dic[REACH_DOWNSTREAM] = downStreamDic[id]
                dic[REACH_UPDOWN_ORDER] = upstreamDownOrderDic[id]
                dic[REACH_DOWNUP_ORDER] = downstreamUpOrderDic[id]
                dic[REACH_MANNING] = dicManning[id]
                dic[REACH_SLOPE] = slopeDic[id]
                dic[REACH_V0] = math.sqrt(slopeDic[id]) * math.pow(depthDic[id], 2./3) / dic[REACH_MANNING]
                dic[REACH_NUMCELLS] = areaDic[id]
                dic[REACH_GROUP] = 1
                dic[REACH_GROUPDIVIDED] = 1
                dic[REACH_WIDTH] = widthDic[id]
                dic[REACH_LENGTH] = lenDic[id]
                dic[REACH_DEPTH] = depthDic[id]
                dic[REACH_AREA] = areaDic[id]*dx*dx
                dic[REACH_BC1] = 0.55
                dic[REACH_BC2] = 1.1
                dic[REACH_BC3] = 0.21
                dic[REACH_BC4] = 0.35
                dic[REACH_RK1] = 1.71
                dic[REACH_RK2] = 50
                dic[REACH_RK3] = 0.36
                dic[REACH_RK4] = 2
                dic[REACH_RS1] = 1
                dic[REACH_RS2] = 0.05
                dic[REACH_RS3] = 0.5
                dic[REACH_RS4] = 0.05
                dic[REACH_RS5] = 0.05
                curFilter = {REACH_SUBBASIN: id}
                db[DB_TAB_REACH].find_one_and_replace(curFilter, dic, upsert = True)
                #db[DB_TAB_REACH].insert_one(dic)
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
            dic[REACH_SUBBASIN] = id
            dic[REACH_DOWNSTREAM] = downStreamDic[id]
            dic[REACH_UPDOWN_ORDER] = upstreamDownOrderDic[id]
            dic[REACH_DOWNUP_ORDER] = downstreamUpOrderDic[id]
            dic[REACH_MANNING] = dicManning[id]
            dic[REACH_V0] = math.sqrt(slopeDic[id]) * math.pow(depthDic[id], 2./3) / dic[REACH_MANNING]
            dic[REACH_NUMCELLS] = areaDic[id]
            dic[REACH_KMETIS] = groupDicK[id]
            dic[REACH_PMETIS] =  groupDicP[id]
            dic[REACH_GROUPDIVIDED] = n
            dic[REACH_WIDTH] = widthDic[id]
            dic[REACH_LENGTH] = lenDic[id]
            dic[REACH_DEPTH] = depthDic[id]
            dic[REACH_AREA] = areaDic[id]*dx*dx
            dic[REACH_BC1] = 0.55
            dic[REACH_BC2] = 1.1
            dic[REACH_BC3] = 0.21
            dic[REACH_BC4] = 0.35
            dic[REACH_RK1] = 1.71
            dic[REACH_RK2] = 50
            dic[REACH_RK3] = 0.36
            dic[REACH_RK4] = 2
            dic[REACH_RS1] = 1
            dic[REACH_RS2] = 0.05
            dic[REACH_RS3] = 0.5
            dic[REACH_RS4] = 0.05
            dic[REACH_RS5] = 0.05
            curFilter = {REACH_SUBBASIN: id}
            db[DB_TAB_REACH].find_one_and_replace(curFilter, dic, upsert = True)
            #db[DB_TAB_REACH].insert_one(dic)
         
    #db[DB_TAB_REACH].create_index(REACH_GROUPDIVIDED)
    db[DB_TAB_REACH].create_index([(REACH_SUBBASIN, pymongo.ASCENDING),
                                   (REACH_GROUPDIVIDED, pymongo.ASCENDING)])
    
    print 'The reaches table is generated!'

if __name__ == "__main__":
    #conn = Connection(host='192.168.6.55', port=27017)
    conn = MongoClient(host='127.0.0.1', port=27017)
    #workingDir = '/data/liujz/data/fenkeng/output'
    workingDir = 'E:\data_m\SEIMS\dianbu_10m_output'
    #db = conn['cluster_model_fenkeng_90m']
    db = conn['model_dianbu_10m_longterm']
    GenerateReachTable(workingDir, db, False)
