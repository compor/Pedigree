# cmake file

find_package(Doxygen)

if(NOT DOXYGEN_FOUND)
  message(STATUS "Could not find Doxygen")

  return()
endif()

set(DOXYGEN_CFG_FILE_TEMPLATE "doxygen.cfg.in")
set(DOXYGEN_CFG_FILE "${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg")

configure_file(${DOXYGEN_CFG_FILE_TEMPLATE} ${DOXYGEN_CFG_FILE} @ONLY)

add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CFG_FILE}
  WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating documentation" VERBATIM)

