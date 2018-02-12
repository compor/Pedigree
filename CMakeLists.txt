# cmake file

# build tool configuration

cmake_minimum_required(VERSION 3.2.0)

# add project cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake-utils/modules")

include(CMakePackageConfigHelpers)
include(HandleLLVMDependency)
include(cmake-utils)
include(DetectLLVMSharedMode)

add_cleanall_target()

option(PEDIGREE_SKIP_TESTS "Skip configuring the project tests" ON)
option(PEDIGREE_DEBUG "Enable project custom debug output facilities" OFF)

message(STATUS "project debug: ${PEDIGREE_DEBUG}")

get_version(SHORT VERSION PRJ_VERSION)

set(PRJ_NAME Pedigree)
string(TOUPPER ${PRJ_NAME} PRJ_NAME_UPPER)
string(TOLOWER ${PRJ_NAME} PRJ_NAME_LOWER)

# part of LLVM or standalone
set(PRJ_STANDALONE_BUILD FALSE)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(${PRJ_NAME} C CXX)

  set(PRJ_STANDALONE_BUILD TRUE)
  set(PEDIGREE_SKIP_TESTS OFF)
  set(PEDIGREE_DEBUG ON)
  set(PEDIGREE_EXPORT ${PRJ_NAME})

  message(STATUS "configuring as external LLVM project")
endif()

if(NOT PEDIGREE_EXPORT)
  message(FATAL_ERROR 
    "PEDIGREE_EXPORT must be defined when built as dependent project")
endif()

file(RELATIVE_PATH PATH_PREFIX ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
if(NOT PATH_PREFIX)
  set(PATH_PREFIX ".")
endif()

# CMP0056 is required for any libc++abi dependency of the try_compile() command
# this bumps required version to 3.2
# otherwise LD_LIBRARY_DIR needs to be set
set_policies(CMP0056 NEW)


# requirements

handle_llvm_dependency(${PRJ_STANDALONE_BUILD})


# project configuration

set(PRJ_SHARED_MODE "STATIC")

if(NOT DEFINED BUILD_SHARED_LIBS)
  detect_llvm_shared_mode(SHARED_MODE shared_mode LLVM_VERSION ${LLVM_VERSION})
  string(TOUPPER ${shared_mode} shared_mode)

  message(STATUS "LLVM shared mode: ${shared_mode}")

  if(NOT ${PRJ_SHARED_MODE} STREQUAL ${shared_mode})
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

if(PRJ_STANDALONE_BUILD)
  set(CMAKE_SKIP_BUILD_RPATH FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  #set(CMAKE_INSTALL_RPATH ${LLVM_LIBRARY_DIRS})

  if(CMAKE_VERSION VERSION_EQUAL "3.8.0" OR CMAKE_VERSION VERSION_GREATER "3.8.0")
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
endif()


# dependencies

find_package(Foo CONFIG)

if(Foo_FOUND)
  set(${PRJ_NAME_UPPER}_USES_FOO 1)
else()
  set(${PRJ_NAME_UPPER}_USES_FOO 0)
  message(WARNING "package Foo was not found; skipping.")
endif()

#

set(LIB_SOURCES "lib/PedigreePass.cpp")

set(LIB_NAME "LLVM${PRJ_NAME}Pass")
set(OBJ_LIB_NAME "${LIB_NAME}_obj")
set(TEST_LIB_NAME "${LIB_NAME}_test")

add_library(${OBJ_LIB_NAME} OBJECT ${LIB_SOURCES})

set_target_properties(${OBJ_LIB_NAME} PROPERTIES 
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)

if(Foo_FOUND)
  target_link_libraries(${OBJ_LIB_NAME} PUBLIC Foo)
endif()

target_compile_definitions(${OBJ_LIB_NAME} PUBLIC ${LLVM_DEFINITIONS})
target_compile_definitions(${OBJ_LIB_NAME} PRIVATE VERSION_STRING=${PRJ_VERSION})

set(PRJ_DEBUG_VALUE 0)
if(PEDIGREE_DEBUG)
  set(PRJ_DEBUG_VALUE 1)
endif()

target_include_directories(${OBJ_LIB_NAME} PUBLIC ${LLVM_INCLUDE_DIRS})
target_include_directories(${OBJ_LIB_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(${OBJ_LIB_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)
target_include_directories(${OBJ_LIB_NAME} PUBLIC
  $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${PATH_PREFIX}/include>)

#

add_library(${LIB_NAME} ${PRJ_SHARED_MODE} $<TARGET_OBJECTS:${OBJ_LIB_NAME}>)
add_library(${TEST_LIB_NAME} STATIC $<TARGET_OBJECTS:${OBJ_LIB_NAME}>)

# according to http://llvm.org/docs/CMake.html
# we do not need the below library dependencies since the plugin will be loaded
# via opt or clang which already have those libs in their dependencies
llvm_map_components_to_libnames(LLVM_LIBS core support analysis passes)

if(LLVM_LIBS)
  list(GET LLVM_LIBS 0 SINGLE_LLVM_LIB)
  get_target_property(LLVM_LIB_TARGET_TYPE ${SINGLE_LLVM_LIB} TYPE)
endif()

if(${LLVM_LIB_TARGET_TYPE} STREQUAL "STATIC_LIBRARY")
  target_link_libraries(${LIB_NAME} PUBLIC ${LLVM_LIBS})
  target_link_libraries(${TEST_LIB_NAME} PUBLIC ${LLVM_LIBS})
endif()

if(nlohmann_json_FOUND)
  target_link_libraries(${LIB_NAME} PRIVATE ${JSON_LIB})
  target_link_libraries(${TEST_LIB_NAME} PRIVATE ${JSON_LIB})
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


if(PRJ_STANDALONE_BUILD)
  attach_compilation_db(TARGET ${LIB_NAME})
endif()


# installation

set(CMAKE_INSTALL_MESSAGE "LAZY")

if(PRJ_STANDALONE_BUILD)
  set(PRJ_INSTALL_INCLUDE_DIR "include")
  set(PRJ_INSTALL_LIB_DIR "lib")
  set(PRJ_INSTALL_SHARE_DIR "share")
else()
set(PRJ_INSTALL_INCLUDE_DIR "${PATH_PREFIX}/include")
  set(PRJ_INSTALL_LIB_DIR "${PATH_PREFIX}/lib")
set(PRJ_INSTALL_SHARE_DIR "${PATH_PREFIX}/share")
endif()

set(PRJ_CONFIG_HEADER_FILE "Config.hpp")
set(PRJ_CMAKE_CONFIG_FILE "config.cmake")
set(PRJ_CMAKE_CONFIG_EXTRA_FILE "config-extra.cmake")

set(DEPENDEE "")

if(Foo_FOUND)
  list(APPEND DEPENDEE ${FOO_LIB_DIR}${TRGT_PREFIX}${FOO_LIB}${TRGT_SUFFIX})
endif()

install(TARGETS ${LIB_NAME} EXPORT ${PEDIGREE_EXPORT}
  ARCHIVE DESTINATION "${PRJ_INSTALL_LIB_DIR}"
  LIBRARY DESTINATION "${PRJ_INSTALL_LIB_DIR}")

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include")

add_custom_target(clean_generated_include
  COMMAND "${CMAKE_COMMAND}" "-E" "remove_directory"
  "${CMAKE_CURRENT_BINARY_DIR}/include")
add_dependencies(clean_all clean_generated_include)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/include/${PRJ_CONFIG_HEADER_FILE}.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/${PRJ_CONFIG_HEADER_FILE}")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/share/cmake/${PRJ_CMAKE_CONFIG_FILE}.in"
  "${CMAKE_CURRENT_BINARY_DIR}/share/cmake/${PRJ_NAME_LOWER}-${PRJ_CMAKE_CONFIG_FILE}"
  @ONLY)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/share/cmake/${PRJ_CMAKE_CONFIG_EXTRA_FILE}.in"
  "${CMAKE_CURRENT_BINARY_DIR}/share/cmake/${PRJ_NAME}-${PRJ_CMAKE_CONFIG_EXTRA_FILE}"
  @ONLY)

get_version(SHORT VERSION PRJ_VERSION_SHORT)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/share/cmake/${PRJ_NAME_LOWER}-config-version.cmake"
  VERSION ${PRJ_VERSION_SHORT} COMPATIBILITY SameMajorVersion)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/" 
  DESTINATION ${PRJ_INSTALL_INCLUDE_DIR}
  PATTERN "*.in" EXCLUDE)
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/" 
  DESTINATION ${PRJ_INSTALL_INCLUDE_DIR})

if(PRJ_STANDALONE_BUILD)
  install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/share/"
    DESTINATION ${PRJ_INSTALL_SHARE_DIR})
endif()

if(PRJ_STANDALONE_BUILD)
  install(EXPORT ${PEDIGREE_EXPORT} DESTINATION "${PRJ_INSTALL_SHARE_DIR}/cmake")
endif()

