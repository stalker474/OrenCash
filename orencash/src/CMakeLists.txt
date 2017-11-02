cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

################### Variables. ####################
# Change if you want modify path or other values. #
###################################################

set(PROJECT_NAME orencash)
# Output Variables
set(OUTPUT_RELEASE Release/bin)

############## CMake Project ################
#        The main options of project        #
#############################################

project(${PROJECT_NAME} CXX)

# Define Release by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "Build type not specified: Use Release by default.")
endif(NOT CMAKE_BUILD_TYPE)

# Definition of Macros
add_definitions(
   -DNDEBUG 
   -D_CONSOLE 
   -DWINVER=0x0A00 
   -D_WIN32_WINNT=0x0A00 
   -DUNICODE
   -D_UNICODE
)

############## Artefacts Output #################
# Defines outputs , depending Debug or Release. #
#################################################

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_REL}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_REL}")
  set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_REL}")
  
  set(BOOST_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../vendor/boost_1_64_0")
  set(ED25519_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../vendor/ed25519/src")
  set(SQLITE3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../vendor/sqlite3")
  set(RESTBED_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../vendor/restbed/source")
  set(RPCLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../vendor/rpclib/include")
  set(CRYPTOPP_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../vendor/cryptopp-5.6.5")
  set(LIBODB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../vendor/odb/libodb-2.4.0")
  set(LIBODB_SQLITE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../vendor/odb/libodb-sqlite-2.4.0")
  
  
  set(BOOST_LIB_DIR "${CMAKE_SOURCE_DIR}/../lib-vs15/x64/")
  set(ED25519_LIB_DIR "${CMAKE_SOURCE_DIR}/../lib-vs15/x64/")
  set(SQLITE3_LIB_DIR "${CMAKE_SOURCE_DIR}/../lib-vs15/x64/")
  set(RESTBED_LIB_DIR "${CMAKE_SOURCE_DIR}/../lib-vs15/x64/")
  set(RPCLIB_LIB_DIR "${CMAKE_SOURCE_DIR}/../lib-vs15/x64/")
  set(CRYPTOPP_LIB_DIR "${CMAKE_SOURCE_DIR}/../lib-vs15/x64/")
  set(LIBODB_LIB_DIR "${CMAKE_SOURCE_DIR}/../lib-vs15/x64/")
  set(LIBODB_SQLITE_LIB_DIR "${CMAKE_SOURCE_DIR}/../lib-vs15/x64/")
endif()

################# Flags ################
# Defines Flags for Windows and Linux. #
########################################

if(MSVC)
   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W3 /EHsc")
   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W3 /EHsc")
endif(MSVC)
if(NOT MSVC)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
   endif()
endif(NOT MSVC)


################ Files ################
#   --   Add files to project.   --   #
#######################################

file(GLOB SRC
    "*.h"
    "*.cpp"
	"*.hxx"
	"*.cxx"
	"*.ixx"
)

include_directories(${BOOST_INCLUDE_DIR})
include_directories(${ED25519_INCLUDE_DIR})
include_directories(${SQLITE3_INCLUDE_DIR})
include_directories(${RESTBED_INCLUDE_DIR})
include_directories(${RPCLIB_INCLUDE_DIR})
include_directories(${CRYPTOPP_INCLUDE_DIR})
include_directories(${LIBODB_INCLUDE_DIR})
include_directories(${LIBODB_SQLITE_INCLUDE_DIR})

link_directories(${BOOST_LIB_DIR})
link_directories(${ED25519_LIB_DIR})
link_directories(${SQLITE3_LIB_DIR})
link_directories(${RESTBED_LIB_DIR})
link_directories(${RPCLIB_LIB_DIR})
link_directories(${CRYPTOPP_LIB_DIR})
link_directories(${LIBODB_LIB_DIR})
link_directories(${LIBODB_SQLITE_LIB_DIR})

# Add executable to build.
add_executable(${PROJECT_NAME} 
   ${SRC}
)

if(MSVC)
   target_link_libraries(${PROJECT_NAME} ed25519.lib sqlite3.lib cryptlib.lib restbed.lib rpc.lib odb.lib odb-sqlite.lib )
endif(MSVC)
