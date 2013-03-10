# Project name is not mandatory, but you should use it
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
find_package(CURL REQUIRED)

# find sqlite3 library - not yet in cmake
find_package(Sqlite3 REQUIRED)
include_directories(${SQLITE3_INCLUDE_DIR})

# Link the executable
target_link_libraries(muttvcardsearch ${CURL_LIBRARY} ${SQLITE3_LIBRARIES})
