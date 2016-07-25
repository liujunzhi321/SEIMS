# This file is used for make SEIMS project in Windows and Linux/Unix.
# Author         : LiangJun Zhu, JunZhi Liu
# First release  : Jun.  4, 2015
# Latest updated : Jul. 10, 2017
# Change notes   : 1. Support Visual Studio in Windows
#                  2. 

cmake_minimum_required(VERSION 2.6)
IF (COMMAND CMAKE_POLICY)
    CMAKE_POLICY(SET CMP0015 NEW)
ENDIF (COMMAND CMAKE_POLICY)

IF (SEIMSVERSION STREQUAL MPI)
    PROJECT(SEIMS_MPI)
    MESSAGE(STATUS "Making SEIMS in MPI version...")
    ADD_DEFINITIONS(-DMONGO_HAVE_STDINT -DUSE_MONGODB -DMODULE_EXPORTS)
ELSE ()
    PROJECT(SEIMS_OMP)
    MESSAGE(STATUS "Making SEIMS in OpenMP version...")
    ADD_DEFINITIONS(-DMONGO_HAVE_STDINT -DUSE_MONGODB -DMODULE_EXPORTS -DMULTIPLY_REACHES)
ENDIF ()
IF (MSVC)
    ADD_DEFINITIONS(-DMSVC)
ENDIF ()
#SET (CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_BUILD_TYPE "Release")
SET(MONGO_HOME ${CMAKE_CURRENT_SOURCE_DIR}/include)
SET(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
IF (WIN32)
    SET(GDAL_HOME ${CMAKE_CURRENT_SOURCE_DIR}/include/gdal)
    SET(BSON_INC ${MONGO_HOME}/mongo-win/libbson-1.0)
    SET(MONGO_INC ${MONGO_HOME}/mongo-win/libmongoc-1.0)
    ## SET MPI
    #For MS-MPI in Windows, version v6 and above.
    set(MPIEXEC "C:/Program Files/Microsoft MPI/Bin/mpiexec.exe")
    #set(MPIEXEC "C:/Program Files/Microsoft HPC Pack 2012/Bin/mpiexec.exe")
    # For building MPI programs the selected Visual Studio compiler is used, namely cl.exe.
    # So there is no need to set a specific MPI compiler.
    # Make sure the correct libraries (64-bit or 32-bit) are selected.
    # Decide between 32-bit and 64-bit libraries for Microsoft's MPI
    if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "AMD64")
        set(MS_MPI_ARCH_DIR x64)
        set(MS_MPI_PATH "C:/Program Files (x86)/Microsoft SDKs/MPI")
    else ()
        set(MS_MPI_PATH "C:/Program Files/Microsoft SDKs/MPI")
        set(MS_MPI_ARCH_DIR x86)
    endif ()
    set(MPI_CXX_INCLUDE_PATH "${MS_MPI_PATH}/Include")
    # Currently, because the GDAL is 32-bit, then only x86 MPI is supported. LJ
    #set(MPI_CXX_LIBRARIES "${MS_MPI_PATH}/Lib/${MS_MPI_ARCH_DIR}/msmpi.lib")
    set(MPI_CXX_LIBRARIES "${MS_MPI_PATH}/Lib/x86/msmpi.lib")
    set(MPI_C_INCLUDE_PATH "${MPI_CXX_INCLUDE_PATH}")
    set(MPI_C_LIBRARIES "${MPI_CXX_LIBRARIES}")
    set(MPI_INCLUDE "${MPI_CXX_INCLUDE_PATH}")
    set(MPI_LIB "${MPI_CXX_LIBRARIES}")
    ## END SETTING MPI
ELSE ()
    SET(GDAL_HOME /soft/share)
    SET(BSON_INC ${MONGO_HOME}/mongo-linux/libbson-1.0)
    SET(MONGO_INC ${MONGO_HOME}/mongo-linux/libmongoc-1.0)
    ## SET MPI, openmpi (/opt/openmpi) or mpich (/home/zhulj/mpich)
    SET(MPI_HOME /home/zhulj/mpich/)
    SET(CMAKE_CXX_COMPILER "/home/zhulj/gcc4.8.4/bin/g++")
    # Set pthreads
    SET (CMAKE_THREAD_PREFER_PTHREAD TRUE)
    FIND_PACKAGE(Threads)
    IF(CMAKE_USE_PTHREADS_INIT)
        SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    ENDIF()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++11 -ldl -fopenmp -fPIC -static-libstdc++ -static-libgcc")
    # Warning messages
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wfatal-errors")
    ADD_DEFINITIONS(-Dlinux)
ENDIF (WIN32)
# Add support for OpenMP
FIND_PACKAGE(OpenMP)
IF (OPENMP_FOUND)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ENDIF ()

SET(BASE_UTIL ./src/base/util)
SET(BASE_DATA ./src/base/data)
SET(BASE_BMPS ./src/base/bmps)
SET(BASE_MODS ./src/base/module_setting)
SET(MODULE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/base/util/SimulationModule.cpp)

INCLUDE_DIRECTORIES(${BSON_INC} ${MONGO_INC} ${GDAL_HOME} ${GDAL_HOME}/include ${BASE_UTIL} ${BASE_DATA} ${BASE_MODS} ${BASE_BMPS})
LINK_DIRECTORIES(${GDAL_HOME}/lib ${LIB_DIR} ${PROJECT_BINARY_DIR})

SET(BASE_HOME ./src/base)
SET(MODULE_HOME ./src/modules)
SET(MAIN_HOME ./src/main)

IF (WIN32)## copy external dll libraries to Debug and Release folder
    SET(DLL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/dlls)
    SET(EXTERNAL_DLLS "${DLL_DIR}/gdal15-vc9.dll" "${DLL_DIR}/libpng13-vc9.dll" "${DLL_DIR}/libexpat.dll" "${DLL_DIR}/libmongoc-1.0.dll" "${DLL_DIR}/zlib1-vc9.dll" "${DLL_DIR}/libbson-1.0.dll")
    FILE(COPY ${EXTERNAL_DLLS} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
    FILE(COPY ${EXTERNAL_DLLS} DESTINATION ${PROJECT_BINARY_DIR}/Release)
    SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
ENDIF ()

ADD_SUBDIRECTORY(${BASE_HOME})
ADD_SUBDIRECTORY(${MODULE_HOME})
ADD_SUBDIRECTORY(${MAIN_HOME})

MESSAGE(STATUS "SEIMS is Ready for you!")
