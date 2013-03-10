project(muttvcardsearch)

# States that CMake required version must be greater than 2.6
# no clue if needed, but all tutorials have something similar...
cmake_minimum_required(VERSION 2.6)

# include custom find_package scripts from cmake/Modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Include the directory itself as a path to include directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# add source files
file(GLOB muttvcardsearch_SOURCES *.cpp)
file(GLOB vcard_SOURCES vCard/*.cpp)

# Create executable
add_executable(muttvcardsearch ${vcard_SOURCES} ${muttvcardsearch_SOURCES})

# find curl library
find_package(CURL)

# find sqlite3 library - not yet in cmake
find_package(Sqlite3 REQUIRED)
include_directories(${SQLITE3_INCLUDE_DIR})

# Link the executable
target_link_libraries(muttvcardsearch ${CURL_LIBRARY} ${SQLITE3_LIBRARIES})

IF( SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY_RELEASE AND SQLITE3_LIBRARY_DEBUG )
    SET(SQLITE3_FIND_QUIETLY TRUE)
ENDIF( SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY_RELEASE AND SQLITE3_LIBRARY_DEBUG )

FIND_PATH( SQLITE3_INCLUDE_DIR sqlite3.h  )

FIND_LIBRARY(SQLITE3_LIBRARY_RELEASE NAMES sqlite3 )

FIND_LIBRARY(SQLITE3_LIBRARY_DEBUG NAMES sqlite3 sqlite3d  HINTS /usr/lib/debug/usr/lib/ )

IF( SQLITE3_LIBRARY_RELEASE OR SQLITE3_LIBRARY_DEBUG AND SQLITE3_INCLUDE_DIR )
	SET( SQLITE3_FOUND TRUE )
ENDIF( SQLITE3_LIBRARY_RELEASE OR SQLITE3_LIBRARY_DEBUG AND SQLITE3_INCLUDE_DIR )

IF( SQLITE3_LIBRARY_DEBUG AND SQLITE3_LIBRARY_RELEASE )
	# if the generator supports configuration types then set
	# optimized and debug libraries, or if the CMAKE_BUILD_TYPE has a value
	IF( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
		SET( SQLITE3_LIBRARIES optimized ${SQLITE3_LIBRARY_RELEASE} debug ${SQLITE3_LIBRARY_DEBUG} )
	ELSE( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
    # if there are no configuration types and CMAKE_BUILD_TYPE has no value
    # then just use the release libraries
		SET( SQLITE3_LIBRARIES ${SQLITE3_LIBRARY_RELEASE} )
	ENDIF( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
ELSEIF( SQLITE3_LIBRARY_RELEASE )
	SET( SQLITE3_LIBRARIES ${SQLITE3_LIBRARY_RELEASE} )
ELSE( SQLITE3_LIBRARY_DEBUG AND SQLITE3_LIBRARY_RELEASE )
	SET( SQLITE3_LIBRARIES ${SQLITE3_LIBRARY_DEBUG} )
ENDIF( SQLITE3_LIBRARY_DEBUG AND SQLITE3_LIBRARY_RELEASE )

IF( SQLITE3_FOUND )
	IF( NOT SQLITE3_FIND_QUIETLY )
		MESSAGE( STATUS "Found Sqlite3 header file in ${SQLITE3_INCLUDE_DIR}")
		MESSAGE( STATUS "Found Sqlite3 libraries: ${SQLITE3_LIBRARIES}")
	ENDIF( NOT SQLITE3_FIND_QUIETLY )
ELSE(SQLITE3_FOUND)
	IF( SQLITE3_FIND_REQUIRED)
		MESSAGE( FATAL_ERROR "Could not find Sqlite3" )
	ELSE( SQLITE3_FIND_REQUIRED)
		MESSAGE( STATUS "Optional package Sqlite3 was not found" )
	ENDIF( SQLITE3_FIND_REQUIRED)
ENDIF(SQLITE3_FOUND)
