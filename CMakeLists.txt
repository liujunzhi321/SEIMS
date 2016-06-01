# This file is used for make Microsoft Visual Studio Project files in Windows and compile executable files in Linux/Unix.
# Author         : LiangJun Zhu, JunZhi Liu
# First release  : Jun. 4, 2015
# Latest updated : May. 3, 2016

cmake_minimum_required(VERSION 2.6)
IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0015 NEW)
ENDIF(COMMAND CMAKE_POLICY)

IF(SEIMSVERSION STREQUAL MPI)
	PROJECT (SEIMS_MPI)
	MESSAGE(STATUS "Making SEIMS in MPI version...")
	ADD_DEFINITIONS(-DMONGO_HAVE_STDINT -DUSE_MONGODB -DMODULE_EXPORTS)
ELSE()
	PROJECT (SEIMS_OMP)
	MESSAGE(STATUS "Making SEIMS in OpenMP version...")
	ADD_DEFINITIONS(-DMONGO_HAVE_STDINT -DUSE_MONGODB -DMODULE_EXPORTS -DMULTIPLY_REACHES)
ENDIF()
#SET (CMAKE_BUILD_TYPE "Debug")
SET (CMAKE_BUILD_TYPE "Release")
SET (MONGO_HOME ${CMAKE_CURRENT_SOURCE_DIR}/include)
IF(WIN32)
	SET (GDAL_HOME ${CMAKE_CURRENT_SOURCE_DIR}/include/gdal)
	SET (LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
	SET (BSON_INC ${MONGO_HOME}/mongo-win/libbson-1.0)
	SET (MONGO_INC ${MONGO_HOME}/mongo-win/libmongoc-1.0)
	## SET MPI
	#For MS-MPI in Windows, version v6 and above.
	set(MPIEXEC "D:/Program Files/Microsoft HPC Pack 2012/Bin/mpiexec.exe")
	# For building MPI programs the selected Visual Studio compiler is used, namely cl.exe.
	# So there is no need to set a specific MPI compiler.
	set(MPI_CXX_INCLUDE_PATH "D:/Program Files/Microsoft HPC Pack 2012/Inc")
	# Make sure the correct libraries (64-bit or 32-bit) are selected.
	# Decide between 32-bit and 64-bit libraries for Microsoft's MPI
	if("${CMAKE_SIZEOF_VOID_P}" EQUAL 8)
		set(MS_MPI_ARCH_DIR x64)
	else()
		set(MS_MPI_ARCH_DIR x86)
	endif()
	set(MPI_CXX_LIBRARIES "D:/Program Files/Microsoft HPC Pack 2012/Lib/${MS_MPI_ARCH_DIR}/msmpi.lib")
	SET(MPI_LIBDIR "D:/Program Files/Microsoft HPC Pack 2012/${MS_MPI_ARCH_DIR}")
	set(MPI_INCLUDE "${MPI_CXX_INCLUDE_PATH}")
	set(MPI_LIB "${MPI_CXX_LIBRARIES}")
	## END SETTING MPI
ELSE()
	SET (CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -fopenmp -fPIC -g -ggdb")
	SET (CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -fopenmp -fPIC")
	SET (BSON_INC ${MONGO_HOME}/mongo-linux/libbson-1.0)
	SET (MONGO_INC ${MONGO_HOME}/mongo-linux/libmongoc-1.0)
	SET (MPI_HOME /usr/lib64/openmpi)
	SET (GDAL_HOME /soft/share)
	SET (LIB_DIR /usr/lib)
ENDIF(WIN32)

SET (BASE_UTIL ./src/base/util)
SET (BASE_DATA ./src/base/data)
SET (BASE_BMPS ./src/base/bmps)
SET (BASE_MODS ./src/base/module_setting)
SET (MODULE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/base/util/SimulationModule.cpp)

INCLUDE_DIRECTORIES(${BSON_INC} ${MONGO_INC} ${GDAL_HOME} ${GDAL_HOME}/include ${BASE_UTIL} ${BASE_DATA} ${BASE_MODS} ${BASE_BMPS})
LINK_DIRECTORIES(${GDAL_HOME}/lib ${LIB_DIR})

SET (BASE_HOME ./src/base)
SET (MODULE_HOME ./src/modules)
SET (MAIN_HOME ./src/main)

IF(WIN32)## copy external dll libraries to Debug and Release folder
	SET(DLL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/dlls)
	SET(EXTERNAL_DLLS "${DLL_DIR}/gdal15-vc9.dll" "${DLL_DIR}/libpng13-vc9.dll" "${DLL_DIR}/libexpat.dll" "${DLL_DIR}/libmongoc-1.0.dll" "${DLL_DIR}/zlib1-vc9.dll" "${DLL_DIR}/libbson-1.0.dll")
	FILE(COPY ${EXTERNAL_DLLS} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
	FILE(COPY ${EXTERNAL_DLLS} DESTINATION ${PROJECT_BINARY_DIR}/Release)
ENDIF(WIN32)
ADD_SUBDIRECTORY(${BASE_HOME})
ADD_SUBDIRECTORY(${MODULE_HOME})
ADD_SUBDIRECTORY(${MAIN_HOME})
IF(WIN32)
	SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
ENDIF()
MESSAGE(STATUS "SEIMS is Ready for you!")
