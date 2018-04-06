# cmake file

# build tool configuration

cmake_minimum_required(VERSION 3.2.0)

list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake-utils/general"
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake-utils/modules")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(HandleLLVMDependency)
include(AddCleanAllTarget)
include(AttachCompilationDB)
include(DetectLLVMSharedMode)
include(GetGitVersion)

add_clean_all_target()

option(PEDIGREE_SKIP_TESTS "Skip configuring the project tests" ON)
option(PEDIGREE_DEBUG "Enable project custom debug output facilities" OFF)

message(STATUS "project debug: ${PEDIGREE_DEBUG}")

set(PRJ_DEBUG_VALUE 0)
if(PEDIGREE_DEBUG)
  set(PRJ_DEBUG_VALUE 1)
endif()

get_git_version(SHORT VERSION PRJ_VERSION)
set(PRJ_VERSION_SHORT ${PRJ_VERSION})

set(PRJ_NAME Pedigree)
string(TOUPPER ${PRJ_NAME} PRJ_NAME_UPPER)
string(TOLOWER ${PRJ_NAME} PRJ_NAME_LOWER)

project(${PRJ_NAME} C CXX)

set(PEDIGREE_SKIP_TESTS OFF)
set(PEDIGREE_DEBUG ON)
set(PEDIGREE_EXPORT ${PRJ_NAME})

# CMP0056 is required for any libc++abi dependency of the try_compile() command
# this bumps required version to 3.2 otherwise LD_LIBRARY_DIR needs to be set
cmake_policy(SET CMP0056 NEW)


# requirements

handle_llvm_dependency(STANDALONE)

detect_llvm_shared_mode(
  SHARED_MODE LLVM_SHARED_MODE
  LLVM_VERSION ${LLVM_PACKAGE_VERSION})

string(TOUPPER ${LLVM_SHARED_MODE} LLVM_SHARED_MODE)
message(STATUS "LLVM shared mode: ${LLVM_SHARED_MODE}")

# project configuration

set(PRJ_SHARED_MODE "STATIC")

if(NOT DEFINED BUILD_SHARED_LIBS)
  if(NOT ${PRJ_SHARED_MODE} STREQUAL ${LLVM_SHARED_MODE})
    set(PRJ_SHARED_MODE "MODULE")
  endif()
else()
  if(BUILD_SHARED_LIBS)
    set(PRJ_SHARED_MODE "MODULE")
  endif()
endif()

message(STATUS "Plugin shared mode: ${PRJ_SHARED_MODE}")

if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

message(STATUS "RTTI support: ${LLVM_ENABLE_RTTI}")

if(NOT LLVM_ENABLE_EH)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
endif()

message(STATUS "EH support: ${LLVM_ENABLE_EH}")


# when we are building the project as standalone, we'd like to leave the install
# RPATH empty so that potentially we can dynamically link against other
# installed LLVM toolchains

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
#set(CMAKE_INSTALL_RPATH ${LLVM_LIBRARY_DIRS})

if(CMAKE_VERSION VERSION_EQUAL "3.8.0"
    OR CMAKE_VERSION VERSION_GREATER "3.8.0")
  set(CMAKE_BUILD_RPATH ${LLVM_LIBRARY_DIRS})
else()
  # unfortunately this also sets the install RPATH for older cmake versions
  #set(PRJ_LINKER_FLAGS "${PRJ_LINKER_FLAGS} -Wl,-rpath ${LLVM_LIBRARY_DIRS}")
endif()

set(PRJ_LINKER_FLAGS "${PRJ_LINKER_FLAGS} -Wl,--disable-new-dtags")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${PRJ_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS
  "${CMAKE_SHARED_LINKER_FLAGS} ${PRJ_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS
  "${CMAKE_MODULE_LINKER_FLAGS} ${PRJ_LINKER_FLAGS}")


# dependencies

find_package(Boost REQUIRED)

add_subdirectory(third_party/enum-flags)


#

set(LIB_SOURCES
  lib/Analysis/Graphs/DependenceGraphs.cpp
  lib/Analysis/Passes/DDGraphPass.cpp
  lib/Analysis/Passes/CDGraphPass.cpp
  lib/Analysis/Passes/MDGraphPass.cpp
  lib/Analysis/Passes/PDGraphPass.cpp
  lib/Analysis/Passes/Printers/Graphviz/DDGraphPrinterPass.cpp
  lib/Analysis/Passes/Printers/Graphviz/CDGraphPrinterPass.cpp
  lib/Analysis/Passes/Printers/Graphviz/MDGraphPrinterPass.cpp
  lib/Analysis/Passes/Printers/Graphviz/PDGraphPrinterPass.cpp)

set(LIB_NAME "LLVM${PRJ_NAME}Pass")
set(OBJ_LIB_NAME "${LIB_NAME}_obj")
set(TEST_LIB_NAME "${LIB_NAME}_test")

list(APPEND PRJ_LIB_NAMES ${OBJ_LIB_NAME} ${LIB_NAME} ${TEST_LIB_NAME})

add_library(${OBJ_LIB_NAME} OBJECT ${LIB_SOURCES})
add_library(${LIB_NAME} ${PRJ_SHARED_MODE} $<TARGET_OBJECTS:${OBJ_LIB_NAME}>)
add_library(${TEST_LIB_NAME} STATIC $<TARGET_OBJECTS:${OBJ_LIB_NAME}>)

# workaround for cmake's current inability to transitively provide requirements
# from an object library to its users
foreach(PRJ_LIB ${PRJ_LIB_NAMES})
  set_target_properties(${PRJ_LIB} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)

  target_compile_definitions(${PRJ_LIB} PUBLIC ${LLVM_DEFINITIONS})
  target_compile_definitions(${PRJ_LIB}
    PRIVATE VERSION_STRING=${PRJ_VERSION})

  target_include_directories(${PRJ_LIB} PUBLIC ${LLVM_INCLUDE_DIRS})
  target_include_directories(${PRJ_LIB} PUBLIC ${Boost_INCLUDE_DIRS})
  target_include_directories(${PRJ_LIB} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
  target_include_directories(${PRJ_LIB} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)
  target_include_directories(${PRJ_LIB} PUBLIC
    $<INSTALL_INTERFACE:include>)
endforeach()

# workaround for OBJECT libraries since they are not first class citizens
# should be able to use target_link_libraries() against an INTERFACE library
target_include_directories(${OBJ_LIB_NAME} PUBLIC
  $<TARGET_PROPERTY:enum-flags::enum-flags,INTERFACE_INCLUDE_DIRECTORIES>)

target_link_libraries(${LIB_NAME} INTERFACE enum-flags::enum-flags)
target_link_libraries(${TEST_LIB_NAME} INTERFACE enum-flags::enum-flags)

# according to http://llvm.org/docs/CMake.html
# we do not need the below library dependencies since the plugin will be loaded
# via opt or clang which already have those libs in their dependencies
llvm_map_components_to_libnames(LLVM_LIBS core support analysis passes)

target_link_libraries(${TEST_LIB_NAME} PUBLIC ${LLVM_LIBS})

if(${LLVM_SHARED_MODE} STREQUAL "STATIC")
  target_link_libraries(${LIB_NAME} PUBLIC ${LLVM_LIBS})
endif()

#

get_property(TRGT_PREFIX TARGET ${TEST_LIB_NAME} PROPERTY PREFIX)
get_property(TRGT_SUFFIX TARGET ${TEST_LIB_NAME} PROPERTY SUFFIX)

if(NOT TRGT_PREFIX)
  set(TRGT_PREFIX ${CMAKE_SHARED_MODULE_PREFIX})
endif()

if(NOT TRGT_SUFFIX)
  set(TRGT_SUFFIX ${CMAKE_SHARED_MODULE_SUFFIX})
endif()

#

set(TESTEE_PREFIX ${TRGT_PREFIX})
set(TESTEE_SUFFIX ${TRGT_SUFFIX})
set(UNIT_TESTEE_LIB ${TEST_LIB_NAME})
set(LIT_TESTEE_LIB ${LIB_NAME})

add_subdirectory(unittests)
add_subdirectory(tests)
add_subdirectory(doc)

attach_compilation_db(TARGET ${LIB_NAME})


# installation

set(CMAKE_INSTALL_MESSAGE "LAZY")

set(PRJ_CONFIG_HEADER_FILE "Config.hpp")
set(PRJ_CMAKE_CONFIG_FILE "config.cmake")
set(PRJ_CMAKE_CONFIG_EXTRA_FILE "config-extra.cmake")

set(DEPENDEE "")

install(TARGETS ${LIB_NAME} EXPORT ${PEDIGREE_EXPORT}
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include")

add_custom_target(clean_generated_include
  COMMAND "${CMAKE_COMMAND}" "-E" "remove_directory"
  "${CMAKE_CURRENT_BINARY_DIR}/include")
add_dependencies(clean_all clean_generated_include)

configure_file(
  "${CMAKE_INSTALL_INCLUDEDIR}/${PRJ_CONFIG_HEADER_FILE}.in"
  "${CMAKE_INSTALL_INCLUDEDIR}/${PRJ_CONFIG_HEADER_FILE}")

configure_file(
  "${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PRJ_CMAKE_CONFIG_FILE}.in"
  "${CMAKE_INSTALL_LIBDIR}/cmake/${PRJ_NAME_LOWER}-${PRJ_CMAKE_CONFIG_FILE}"
  @ONLY)

configure_file(
  "${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PRJ_CMAKE_CONFIG_EXTRA_FILE}.in"
  "${CMAKE_INSTALL_LIBDIR}/cmake/${PRJ_NAME}-${PRJ_CMAKE_CONFIG_EXTRA_FILE}"
  @ONLY)

write_basic_package_version_file(
  "${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PRJ_NAME_LOWER}-config-version.cmake"
  VERSION "${PRJ_VERSION_SHORT}" COMPATIBILITY SameMinorVersion)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  PATTERN "*.in" EXCLUDE)

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/share/"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}")

#

install(DIRECTORY "third_party/enum-flags/include/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/third_party/enum-flags/include/")

#

install(EXPORT ${PEDIGREE_EXPORT}
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake")

