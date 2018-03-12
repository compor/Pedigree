# cmake file

if(PEDIGREE_SKIP_TESTS)
  message(STATUS "Testing is disabled; skipping unit tests")

  return()
endif()


include(FindPythonModule)

find_python_module(lit)

if(NOT PY_LIT_FOUND)
  message(WARNING "Could not find lit; skipping tests")

  return()
endif()

if(NOT EXISTS ${LLVM_TOOLS_BINARY_DIR}/FileCheck)
  message(WARNING "Could not find FileCheck; skipping tests")

  return()
endif()

# configuration

# aggregate unit test targets under a pseudo-target
add_custom_target(check)

set(PRJ_TEST_CONFIG_FILE "lit.cfg")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${PRJ_TEST_CONFIG_FILE}.in"
  ${PRJ_TEST_CONFIG_FILE})

add_custom_target(lit_tests
  COMMAND ${PYTHON_EXECUTABLE} -m lit.main "${CMAKE_CURRENT_BINARY_DIR}" -v)

add_dependencies(lit_tests ${LIT_TESTEE_LIB})

add_dependencies(check lit_tests)

