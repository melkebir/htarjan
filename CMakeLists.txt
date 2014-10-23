CMAKE_MINIMUM_REQUIRED( VERSION 2.8.3 )

PROJECT( h_tarjan )

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

# find required python packages
FIND_PACKAGE(Boost COMPONENTS python REQUIRED )
MESSAGE( STATUS "found boost:"
  "\nINCLUDE: ${Boost_INCLUDE_DIRS}"
  "\nLIB: ${Boost_LIBRARIES}"
)

FIND_PACKAGE(PythonInterp REQUIRED)
FIND_PACKAGE(PythonLibsNew REQUIRED)
FIND_PACKAGE(NumPy REQUIRED)

if (NOT WIN32)
  add_definitions(-fPIC)
endif()

# custom macro with most of the redundant code for making a python example module
MACRO( addPythonExe _name _srccpp )
  ADD_EXECUTABLE(${_name} ${_srccpp})

  # make the pyd library link against boost_numpy python and boost
  TARGET_LINK_LIBRARIES(${_name} ${CommonLibs})
ENDMACRO()

MACRO( addPythonMod _name _srccpp )
  PYTHON_ADD_MODULE(${_name} ${_srccpp} ${HTarjan_Src})

  # make the pyd library link against boost_numpy python and boost
  TARGET_LINK_LIBRARIES(${_name} ${CommonLibs})
ENDMACRO()

SET( HTarjan_Src
  src/hd.cpp
  src/naivehd.cpp
  src/tarjanhd.cpp
)

SET( HTarjan_Hdr
  src/hd.h
  src/naivehd.h
  src/tarjanhd.h
)

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings -std=c++11" )

set( LIBLEMON_ROOT "$ENV{HOME}/lemon" CACHE PATH "Additional search directory for lemon library" )
set( LIBBOOST_NUMPY_ROOT "$ENV{HOME}/Boost.NumPy" CACHE PATH "Additional search directory for Boost.NumPy library" )

# global settings for include paths
include_directories(
  "${PROJECT_SOURCE_DIR}/src"
  ${PYTHON_INCLUDE_DIRS}
  ${NUMPY_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS} 
  "${LIBLEMON_ROOT}/include"
  "${LIBBOOST_NUMPY_ROOT}/include"
)

link_directories( "${LIBLEMON_ROOT}/lib" "${LIBBOOST_NUMPY_ROOT}/lib" ${Boost_LIBRARY_DIRS} )
set( CommonLibs
	emon
  boost_python
  boost_numpy
  ${Boost_LIBRARIES} ${PYTHON_LIBRARIES}
)

ADD_EXECUTABLE( htarjan ${HTarjan_Hdr} ${HTarjan_Src} src/main.cpp )
TARGET_LINK_LIBRARIES( htarjan ${CommonLibs} )

addPythonMod(hierarchical_decomposition src/python.cpp )
