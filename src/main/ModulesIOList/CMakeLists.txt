INCLUDE_DIRECTORIES(../../base/util ../../base/data ../../base/module_setting)
LINK_DIRECTORIES(${PROJECT_BINARY_DIR})
SET(SRC_LIST main.cpp ModulesIOList.cpp)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
ADD_EXECUTABLE(modulesiolist ${SRC_LIST})
TARGET_LINK_LIBRARIES(modulesiolist util data module_setting)
SET_TARGET_PROPERTIES(modulesiolist PROPERTIES COMPILE_FLAGS "/openmp")
SET_PROPERTY(TARGET modulesiolist PROPERTY FOLDER "main")