CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( h_tarjan )

SET( HTarjan_Src
  src/naivehd.cpp
  src/main.cpp
  src/tarjanhd.cpp
)

SET( HTarjan_Hdr
  src/hd.h
  src/naivehd.h
  src/tarjanhd.h
)

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings" )
SET( CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libstdc++" )

if( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++ " )
endif()

set( LIBLEMON_ROOT "$ENV{HOME}/lemon" CACHE PATH "Additional search directory for lemon library" )

include_directories( "${LIBLEMON_ROOT}/include" "src" )
link_directories( "${LIBLEMON_ROOT}/lib" )
set( CommonLibs
	emon
)

ADD_EXECUTABLE( htarjan ${HTarjan_Hdr} ${HTarjan_Src} ${CommonObjs} )
TARGET_LINK_LIBRARIES( htarjan ${CommonLibs} )
