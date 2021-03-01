cmake_minimum_required(VERSION 3.10)
project(ipo_cv)

#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(BUILD_DEPS "YES" CACHE BOOL "If YES, will install dependencies into sandbox.  Automatically reset to NO after dependencies are installed.")
set(PROJECT_OUTPUT_DIR  ${PROJECT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIR}/bin)  ## mkdir bin
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIR}/lib)  ## mkdir lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIR}/lib)

# Build PDB file
# https://stackoverflow.com/questions/28178978/how-to-generate-pdb-files-for-release-build-with-cmake-flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
#set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")


#set(Boost_LIBRARY_DIRS /usr/local/lib)
#message("Opencv Found --opencv_version  " ${OpenCV_VERSION})
# g++ setting
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GNULINUX") #setting C++ compile
set("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")  # using GCC
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finline-limit=10000")
  if(CMAKE_COMPILER_IS_GNUCC)
     execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if(GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
      message(STATUS "GCC Version >= 4.7")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    else()
      message(STATUS "GCC Version < 4.7")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
    endif()
  endif()

# find package all required
#---------WINDOWS---------
IF (WIN32)
## OpenCV
set(OpenCV 3.4.9  $ENV{THIRD_PARTY}/opencv/)
set(OpenCV_LIBS $ENV{THIRD_PARTY}/opencv/build/x64/vc15/lib)
set(OpenCV_INCLUDE_DIRS $ENV{THIRD_PARTY}/opencv/build/include)
include_directories( ${OpenCV_INCLUDE_DIRS} )
link_directories (${OpenCV_LIBS})
message("OpenCV_INCLUDE_DIRS : " ${OpenCV_INCLUDE_DIRS})
message("OpenCV_LIBS : " ${OpenCV_LIBS})
## BOOST
#set(BOOST_ROOT $ENV{THIRD_PARTY}/Boost/boost_1_66_0)
#set(BOOST_LIBS ${BOOST_ROOT}/lib)
#set(BOOST_INCLUDE_DIRS ${BOOST_ROOT}/include)
#include_directories( ${BOOST_INCLUDE_DIRS} )
#link_directories (${BOOST_LIBS})
#message("BOOST_INCLUDE_DIRS : " ${BOOST_INCLUDE_DIRS})
#message("BOOST_LIBS : " ${BOOST_LIBS})
## Breakpad
set(BREAKPAD_ROOT $ENV{THIRD_PARTY}/breakpad)
set(BREAKPAD_INCLUDE_DIR ${BREAKPAD_ROOT}/include)
set(BREAKPAD_LIBS ${BREAKPAD_ROOT}/lib)
include_directories(${BREAKPAD_INCLUDE_DIR})
link_directories(${BREAKPAD_LIBS})
message("BREAKPAD_INCLUDE_DIR : " ${BREAKPAD_INCLUDE_DIR})
message("BREAKPAD_LIBS : " ${BREAKPAD_LIBS})
#---------Linux---------
ELSE()
## OpenCV
find_package(OpenCV 3.4.9 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
## BOOST
#find_package(Boost 1.55.0 COMPONENTS thread)
#include_directories(${Boost_INCLUDE_DIRS})
#LINK_DIRECTORIES(${Boost_LIBRARY_DIRS}) 

ENDIF()

# code
include_directories(${CMAKE_SOURCE_DIR}/include)
aux_source_directory(./src SRC)
aux_source_directory(./src/component SRC_COMPONENT)
aux_source_directory(./src/error_code SRC_ERRORCODE)

# output binary
add_executable(${PROJECT_NAME} main.cpp ${SRC} ${SRC_COMPONENT} ${SRC_ERRORCODE})
# output so
#add_library( ${PROJECT_NAME} SHARED ${SRC} ${SRC_COMPONENT} ${SRC_ERRORCODE})
#add_library( ${PROJECT_NAME} STATIC ${SRC} ${SRC_COMPONENT} ${SRC_ERRORCODE})
# link library

IF (WIN32)
  target_link_libraries(${PROJECT_NAME} opencv_world349.lib libbreakpad.lib libbreakpad_client.lib)
  # libbreakpad.lib libbreakpad_client.lib 
  # boost_system-vc141-mt-x64-1_66.lib libboost_filesystem-vc141-mt-x64-1_66.lib libboost_regex-vc141-mt-x64-1_66.lib
ELSE()
  target_link_libraries(${PROJECT_NAME} -lstdc++fs ${OpenCV_LIBS})
  # -lgomp -lboost_system -lboost_filesystem -lboost_regex ${Boost_LIBRARIES}
ENDIF()

