MESSAGE(STATUS "    Compiling Selected TauDEM...")
IF(WIN32)
	INCLUDE_DIRECTORIES(${MPI_INCLUDE})
	LINK_DIRECTORIES(${MPI_LIBDIR})
	LINK_LIBRARIES(${MPI_LIB})
ELSE()
	SET(CMAKE_CXX_COMPILER ${MPI_HOME}/bin/mpic++)
	INCLUDE_DIRECTORIES(${MPI_HOME}/include)
	LINK_DIRECTORIES(${MPI_HOME}/lib)
ENDIF()
set (shape_srcs
	shapelib/shapefil.h
	shapelib/dbfopen.c
	shapelib/safileio.c
	shapelib/shpopen.c
	shapelib/shptree.c
	ReadOutlets.cpp)

set (common_srcs commonLib.cpp tiffIO.cpp)
set (D8FILES aread8mn.cpp aread8.cpp ${common_srcs} ${shape_srcs})
set (D8 D8FlowDirmn.cpp d8.cpp Node.cpp ${common_srcs} ${shape_srcs})
set (DINF DinfFlowDirmn.cpp dinf.cpp Node.cpp ${common_srcs} ${shape_srcs})
set (DROPANALYSISFILES DropAnalysis.cpp DropAnalysismn.cpp ${common_srcs} ${shape_srcs})
set (MVOUTLETSTOSTRMFILES moveoutletstostrm.cpp moveoutletstostrmmn.cpp
     ${common_srcs} ${shape_srcs})
set (PITREMOVE flood.cpp PitRemovemn.cpp ${common_srcs})
set (THRESHOLD Threshold.cpp Thresholdmn.cpp ${common_srcs})
set (PEUKERDOUGLAS PeukerDouglas.cpp PeukerDouglasmn.cpp ${common_srcs})
set (STREAMNET streamnetmn.cpp streamnet.cpp ${common_srcs} ${shape_srcs})

add_executable (aread8 ${D8FILES})
add_executable (d8flowdir ${D8})
add_executable (dinfflowdir ${DINF})
add_executable (dropanalysis ${DROPANALYSISFILES})
add_executable (moveoutletstostreams ${MVOUTLETSTOSTRMFILES})
add_executable (pitremove ${PITREMOVE})
add_executable (threshold ${THRESHOLD})
add_executable (peukerdouglas ${PEUKERDOUGLAS})
add_executable (streamnet ${STREAMNET})
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
IF(WIN32)
	SET_PROPERTY(TARGET pitremove d8flowdir dinfflowdir aread8 threshold moveoutletstostreams peukerdouglas streamnet dropanalysis  PROPERTY FOLDER "TauDEM")
ENDIF()