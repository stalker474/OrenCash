cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

################### Variables. ####################
# Change if you want modify path or other values. #
###################################################

set(PROJECT_NAME orencash)
# Output Variables
set(OUTPUT_RELEASE Release/bin)

#
# Build Options
#
option( BUILD_WITH_ODB   "Build persistent version."        OFF )
option( BUILD_TESTS    "Build all available test suites."   OFF )
#
# Configuration
#
include( "${PROJECT_SOURCE_DIR}/cmake/configuration.cmake" )

configure_file("${PROJECT_SOURCE_DIR}/Config.h.in" "${PROJECT_BINARY_DIR}/Config.h")

include_directories(${PROJECT_BINARY_DIR})

#
# Dependencies
#
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/modules" )

find_package( boost-serialization REQUIRED )
include_directories( ${boost-serialization_INCLUDE} )

find_package( ed25519 REQUIRED )
include_directories( ${ed25519_INCLUDE} )

find_package( restbed REQUIRED )
include_directories( ${restbed_INCLUDE} )
link_directories(${restbed_INCLUDE}/../)

find_package( rpclib REQUIRED )
include_directories( ${rpclib_INCLUDE} )

find_package( cryptopp REQUIRED )
include_directories( ${cryptopp_INCLUDE} )

if(BUILD_WITH_ODB)

find_package( libodb REQUIRED )
include_directories( ${libodb_INCLUDE} )

find_package( libodb-sqlite REQUIRED )
include_directories( ${libodb-sqlte_INCLUDE} )

find_package( sqlite3 REQUIRED )
include_directories( ${sqlite3_INCLUDE} )

endif(BUILD_WITH_ODB)

if(NOT MSVC)
include_directories(${LIBODB_BIN_INCLUDE_DIR})
endif()

# Add executable to build.
add_executable(${PROJECT_NAME} 
   ${MANIFEST_SOURCES}
   ${MANIFEST_HEADERS}
   ${ED25519_INCLUDE}
)

if(MSVC)
    target_link_libraries(${PROJECT_NAME} ed25519.lib cryptlib.lib restbed.lib rpc.lib )
    if(BUILD_WITH_ODB)
        target_link_libraries(${PROJECT_NAME} sqlite3.lib odb.lib odb-sqlite.lib )
    endif(BUILD_WITH_ODB)
endif(MSVC)

if(NOT MSVC)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} Threads::Threads)
    target_link_libraries(${PROJECT_NAME} libboost_serialization.a libcryptopp.a librpc.a librestbed.a)
    if(BUILD_WITH_ODB)
        target_link_libraries(${PROJECT_NAME} libodb.a libodb-sqlite.a)
    endif(BUILD_WITH_ODB)
endif(NOT MSVC)
