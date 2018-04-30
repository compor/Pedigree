# cmake file

# requirements

if(PEDIGREE_SKIP_TESTS)
  message(STATUS "Testing is disabled; skipping unit tests")

  return()
endif()

enable_testing()

find_package(GTest)

if(NOT GTEST_FOUND)
  message(WARNING "Could not find GTest; skipping unit tests")

  return()
endif()

find_package(Threads REQUIRED)

# configuration

# aggregate unit test targets under a pseudo-target
add_custom_target(unittests)

set(PRJ_TEST_NAME Test${PRJ_NAME})

set(TEST_SOURCES
  TestPedigreeOpt.cpp
  GenericDependenceGraphTests.cpp
  DDGraphTests.cpp
  PostDominanceFrontierTests.cpp
  CDGraphTests.cpp)

add_executable(${PRJ_TEST_NAME} ${TEST_SOURCES})
add_sanitizers(${PRJ_TEST_NAME})

if(NOT LLVM_ENABLE_EH)
  target_compile_definitions(${PRJ_TEST_NAME} PUBLIC BOOST_NO_EXCEPTIONS)
endif()

target_include_directories(${PRJ_TEST_NAME} PUBLIC include)
target_include_directories(${PRJ_TEST_NAME} PUBLIC ${GTEST_INCLUDE_DIRS})
target_include_directories(${PRJ_TEST_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(${PRJ_TEST_NAME} PUBLIC ${GTEST_BOTH_LIBRARIES})
target_link_libraries(${PRJ_TEST_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT})

llvm_map_components_to_libnames(UNIT_LLVM_LIBS asmparser)

target_link_libraries(${PRJ_TEST_NAME} PUBLIC ${UNIT_LLVM_LIBS})
target_link_libraries(${PRJ_TEST_NAME} PUBLIC ${UNIT_TESTEE_LIB})

# exclude unit test targets from main build
set_target_properties(${PRJ_TEST_NAME} PROPERTIES EXCLUDE_FROM_ALL TRUE)

set_target_properties(${PRJ_TEST_NAME} PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)

add_dependencies(unittests ${PRJ_TEST_NAME})

get_property(PRJ_UNIT_TESTS_EXE TARGET ${PRJ_TEST_NAME} PROPERTY NAME)

set(TEST_DATA_DIR "${CMAKE_CURRENT_BINARY_DIR}/data")
file(MAKE_DIRECTORY ${TEST_DATA_DIR})

add_custom_command(TARGET unittests PRE_BUILD COMMAND
  ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/data/ ${TEST_DATA_DIR})

