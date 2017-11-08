############## CMake Project ################
#        The main options of project        #
#############################################

project(${PROJECT_NAME} CXX)

# Define Release by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "Build type not specified: Use Release by default.")
endif(NOT CMAKE_BUILD_TYPE)

if( NOT WIN32 )
    string( ASCII 27 Esc )

    set( Reset   "${Esc}[m"   )
    set( Red     "${Esc}[31m" )
    set( Blue    "${Esc}[34m" )
    set( Green   "${Esc}[32m" )
    set( Yellow  "${Esc}[33m" )
endif( )

set( INCLUDE_DIR "${PROJECT_SOURCE_DIR}/src" )
set( SOURCE_DIR "${PROJECT_SOURCE_DIR}" )

############## Artefacts Output #################
# Defines outputs , depending Debug or Release. #
#################################################

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_REL}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_REL}")
  set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_REL}")
endif()

 set(CMAKE_CXX_STANDARD 14)

################# Flags ################
# Defines Flags for Windows and Linux. #
########################################

if(MSVC)
   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W3 /EHsc")
   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W3 /EHsc")
endif(MSVC)
if(NOT MSVC)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
   if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
   endif()
endif(NOT MSVC)

################ Files ################
#   --   Add files to project.   --   #
#######################################

if(BUILD_WITH_ODB)

set( MANIFEST_HEADERS
    ${SOURCE_DIR}/Block.h
    ${SOURCE_DIR}/Blockchain.h
    ${SOURCE_DIR}/Config.h
    ${SOURCE_DIR}/CryptoHelpers.h
    ${SOURCE_DIR}/Database.h
    ${SOURCE_DIR}/HTTPServer.h
    ${SOURCE_DIR}/Miner.h
    ${SOURCE_DIR}/Node.h
    ${SOURCE_DIR}/Operator.h
    ${SOURCE_DIR}/PersistentObjects.h
    ${SOURCE_DIR}/PersistentObjects-odb.hxx
)
set( MANIFEST_SOURCES
    ${SOURCE_DIR}/Block.cpp
    ${SOURCE_DIR}/Blockchain.cpp
    ${SOURCE_DIR}/Database.cpp
    ${SOURCE_DIR}/HTTPServer.cpp
    ${SOURCE_DIR}/Miner.cpp
    ${SOURCE_DIR}/Node.cpp
    ${SOURCE_DIR}/Operator.cpp
    ${SOURCE_DIR}/orencash.cpp
    ${SOURCE_DIR}/RPC.cpp
    ${SOURCE_DIR}/Transaction.cpp
    ${SOURCE_DIR}/Wallet.cpp
    ${SOURCE_DIR}/PersistentObjects-odb.cxx
)
endif(BUILD_WITH_ODB)
if(NOT BUILD_WITH_ODB)

set( MANIFEST_HEADERS
    ${SOURCE_DIR}/Block.h
    ${SOURCE_DIR}/Blockchain.h
    ${SOURCE_DIR}/Config.h
    ${SOURCE_DIR}/CryptoHelpers.h
    ${SOURCE_DIR}/Database.h
    ${SOURCE_DIR}/HTTPServer.h
    ${SOURCE_DIR}/Miner.h
    ${SOURCE_DIR}/Node.h
    ${SOURCE_DIR}/Operator.h
)
set( MANIFEST_SOURCES
    ${SOURCE_DIR}/Block.cpp
    ${SOURCE_DIR}/Blockchain.cpp
    ${SOURCE_DIR}/Database.cpp
    ${SOURCE_DIR}/HTTPServer.cpp
    ${SOURCE_DIR}/Miner.cpp
    ${SOURCE_DIR}/Node.cpp
    ${SOURCE_DIR}/Operator.cpp
    ${SOURCE_DIR}/orencash.cpp
    ${SOURCE_DIR}/RPC.cpp
    ${SOURCE_DIR}/Transaction.cpp
    ${SOURCE_DIR}/Wallet.cpp
)

endif(NOT BUILD_WITH_ODB)
message("CONFIGURATION")
message("Build type : ${CMAKE_BUILD_TYPE}")
message("Compiler id : ${CMAKE_CXX_COMPILER_ID}")
