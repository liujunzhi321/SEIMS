# This file is used for make Microsoft Visual Studio Project files and compile executable files in Linux/Unix.
# This is preprocessing for SEIMS.
# First released : Liangjun Zhu
# Date           : Jun. 19, 2015
cmake_minimum_required(VERSION 2.6)
IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0015 NEW)
ENDIF(COMMAND CMAKE_POLICY)
PROJECT (SEIMS_Preprocess)
#SET (CMAKE_BUILD_TYPE "Debug")
SET (CMAKE_BUILD_TYPE "Release")
IF(WIN32)
	## SET MPI
	#For MS-MPI in Windows, version v6 and above.
	set(MPIEXEC "C:/Program Files/Microsoft MPI/Bin/mpiexec.exe")
	# For building MPI programs the selected Visual Studio compiler is used, namely cl.exe.
	# So there is no need to set a specific MPI compiler.
	set(MPI_CXX_INCLUDE_PATH "C:/Program Files (x86)/Microsoft SDKs/MPI/Include")
	# Make sure the correct libraries (64-bit or 32-bit) are selected.
	# Decide between 32-bit and 64-bit libraries for Microsoft's MPI
	if("${CMAKE_SIZEOF_VOID_P}" EQUAL 8)
		set(MS_MPI_ARCH_DIR x64)
	else()
		set(MS_MPI_ARCH_DIR x86)
	endif()
	set(MPI_CXX_LIBRARIES "C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/${MS_MPI_ARCH_DIR}/msmpi.lib")
	SET(MPI_LIBDIR "C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/${MS_MPI_ARCH_DIR}")
	set(MPI_INCLUDE "${MPI_CXX_INCLUDE_PATH}")
	set(MPI_LIB "${MPI_CXX_LIBRARIES}")
	## END SETTING MPI
ELSE()
	SET (CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -fopenmp -fPIC -g -ggdb")
	SET (CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -fopenmp -fPIC")
	SET (MPI_HOME /usr/lib64/openmpi)
	SET (CMAKE_CXX_COMPILER ${MPI_HOME}/bin/mpic++)
	SET (GDAL_HOME /soft/share)
	SET (MONGO_LIB_HOME /soft/share/mongo-c-driver-f06669b)
	SET (LIB_DIR /usr/lib)
ENDIF(WIN32)

SET (PREPROCESS_HOME ./cpp_src)
ADD_SUBDIRECTORY(${PREPROCESS_HOME})
IF(WIN32)
	SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
## copy external dll libraries to Debug and Release folder
	SET(EXTERNAL_DLLS "../lib/dlls/gdal15-vc9.dll" "../lib/dlls/libpng13-vc9.dll" "../lib/dlls/libexpat.dll" "../lib/dlls/zlib1-vc9.dll" "../lib/dlls/mongoc.dll" "../lib/dlls/bson.dll")
	FILE(COPY ${EXTERNAL_DLLS} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
	FILE(COPY ${EXTERNAL_DLLS} DESTINATION ${PROJECT_BINARY_DIR}/Release)
ENDIF()
MESSAGE(STATUS "Preprocessing program for SEIMS is Ready for you!")