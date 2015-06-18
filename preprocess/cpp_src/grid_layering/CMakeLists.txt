ADD_DEFINITIONS(-DMONGO_HAVE_STDINT -DUSE_MONGODB)
INCLUDE_DIRECTORIES(/soft/share/include /soft/share/mongo-c-driver-f06669b/src ./)
LINK_DIRECTORIES(/soft/share/lib /soft/share/mongo-c-driver-f06669b ./)

ADD_EXECUTABLE(grid_layering LayeringFromSource.cpp LayeringFromOutlet.cpp main.cpp Data.cpp CompressIndex.cpp FlowInOut.cpp)

TARGET_LINK_LIBRARIES(grid_layering libgdal.so libmongoc.so libbson.so)
