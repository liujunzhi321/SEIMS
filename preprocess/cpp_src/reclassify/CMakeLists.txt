SET(CMAKE_CXX_COMPILER mpic++)
INCLUDE_DIRECTORIES(/soft/share/include ./)
LINK_DIRECTORIES(/soft/share/lib ./)

ADD_EXECUTABLE(reclassify application.cpp reclassify.cpp ReclassifyOperator.cpp)

TARGET_LINK_LIBRARIES(reclassify libgdal.so )
