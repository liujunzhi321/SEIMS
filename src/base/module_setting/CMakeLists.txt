SET (GDAL_HOME ../../../include/gdal)
SET (MONGO_HOME ../../../include/mongo-c-driver)
SET (LIB_DIR ../../../lib)
INCLUDE_DIRECTORIES(${GDAL_HOME}/include ../util ../data ../bmps ${MONGO_HOME})
LINK_DIRECTORIES(${LIB_DIR} ${PROJECT_BINARY_DIR})
AUX_SOURCE_DIRECTORY(. SRC_LIST)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
ADD_LIBRARY(module_setting STATIC ${SRC_LIST})
TARGET_LINK_LIBRARIES(module_setting util data bmps)
SET_TARGET_PROPERTIES( module_setting PROPERTIES COMPILE_FLAGS "/openmp")
SET_PROPERTY(TARGET module_setting PROPERTY FOLDER "base")


# TARGET_LINK_LIBRARIES(module_setting libutil.so libdata.so libbmps.so libbson.so libmongoc.so)
