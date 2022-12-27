#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(monte)
#----------------------------------------------------------------------------
# Create the variable for the utility directory.
#
#set(UTILITIES_PATH $ENV{HOME}/baltig/read-soft/utilities)

#find_package(ROOT)
find_package(ROOT REQUIRED) # COMPONENTS RIO Net)
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
include(${ROOT_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(./include)

file(GLOB sources ./src/*.cc)

file(GLOB headers ./include/*.hh ./include/*.h)

 
#----------------------------------------------------------------------------
# Add the executable
#
add_executable(monte main.cc ${sources} ${headers})
#target_link_libraries(readCosmic ${UTILITIES_PATH}/libs/libftd3xx.so)
target_link_libraries(monte PUBLIC ${ROOT_LIBRARIES})

target_compile_options(monte PRIVATE -Wall -O3)
#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
#install(TARGETS readCosmic DESTINATION bin)
