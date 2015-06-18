INCLUDE_DIRECTORIES(/soft/share/include ./)
LINK_DIRECTORIES(/soft/share/lib ./)

ADD_EXECUTABLE(mask_raster Raster.cpp mask_rasters.cpp)

TARGET_LINK_LIBRARIES(mask_raster libgdal.so)
