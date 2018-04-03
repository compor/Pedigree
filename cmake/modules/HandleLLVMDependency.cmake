# cmake file

include(CMakeParseArguments)

macro(handle_llvm_dependency)
  set(options STANDALONE)
  set(oneValueArgs)
  set(multiValueArgs)

  cmake_parse_arguments(HLD "${options}" "${oneValueArgs}"
    "${multiValueArgs}" ${ARGN})

  if(NOT HLD_STANDALONE)
    set(HLD_STANDALONE FALSE)
  endif()

  if(HLD_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR
      "${HLD_UNPARSED_ARGUMENTS} extraneous arguments provided")
  endif()

  if(HLD_STANDALONE)
    # find_package in CONFIG mode will lookup and include LLVMConfig.cmake
    find_package(LLVM REQUIRED CONFIG)

    if(NOT LLVM_FOUND)
      message(FATAL_ERROR "Could not find LLVM")
    endif()

    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
  endif()

  if(LLVM_PACKAGE_VERSION VERSION_LESS "3.8.0")
    # this follows the suggestion documented in
    # http://llvm.org/docs/CMake.html#cmake-out-of-source-pass
    # for allowing future inclusion of the plugin in the LLVM source tree
    # by using LLVM's internal cmake functions and utilities
    set(LLVM_RUNTIME_OUTPUT_INTDIR
      "${CMAKE_BINARY_DIR}/bin/${CMAKE_CFG_INT_DIR}")
    set(LLVM_LIBRARY_OUTPUT_INTDIR
      "${CMAKE_BINARY_DIR}/lib/${CMAKE_CFG_INT_DIR}")
  endif()

  # Required if these functions are used:
  # - add_llvm_loadable_module()
  # - add_llvm_library
  if(HLD_STANDALONE)
    list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
  else()
    if(LLVM_PACKAGE_VERSION VERSION_LESS "3.9.0")
      list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/share/llvm/cmake/")
    else()
      list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/lib/cmake/llvm/")
    endif()
  endif()

  include(AddLLVM)
  include(HandleLLVMOptions)
endmacro()

