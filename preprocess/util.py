#! /usr/bin/env python
#coding=utf-8
## @Utility functions
#
#
import math,datetime,time

## Solar Radiation Calculation
#  @param doy day of year
#  @param n   sunshine duration
#  @param lat latitude of sites
#  invoke   Rs(doy, n, lat)
# day of year
def doy(dt):
    sec = time.mktime(dt.timetuple())
    t = time.localtime(sec)
    return t.tm_yday

#earth-sun distance 
def dr(doy):
    return 1 + 0.033*math.cos(2*math.pi*doy/365)

#declination
def dec(doy):
    return 0.409*math.sin(2*math.pi*doy/365 - 1.39)

#sunset hour angle
def ws(lat, dec):
    x = 1 - math.pow(math.tan(lat), 2)*math.pow(math.tan(dec), 2)
    if x < 0:
        x = 0.00001
    #print x
    return 0.5*math.pi - math.atan(-math.tan(lat)*math.tan(dec)/math.sqrt(x))

#solar radiation
def Rs(doy, n, lat):
    """n is sunshine duration"""
    lat = lat * math.pi / 180.
    a = 0.25
    b = 0.5
    d = dec(doy)
    w = ws(lat, d)
    N = 24*w/math.pi
    #Extraterrestrial radiation for daily periods
    ra = (24*60*0.082*dr(doy)/math.pi)*(w*math.sin(lat)*math.sin(d) + math.cos(lat)*math.cos(d)*math.sin(w))
    return (a + b*n/N)*ra