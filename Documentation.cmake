## Doxygen Documentation ##
# Build option, defaults to on if Doxygen is found.
find_package(Doxygen)
option(BUILD_DOCUMENTATION
  "Create the HTML base API documentation (requires Doxygen)"
  ${DOXYGEN_FOUND})

# Documentation build targets.
if (BUILD_DOCUMENTATION)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is required to build documentation.")
  endif (NOT DOXYGEN_FOUND)

  set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/Documentation)

  configure_file(${DOXYFILE_IN} ${DOXYFILE} @ONLY)

  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating HTML API documentation with Doxygen"
    VERBATIM
  )
endif (BUILD_DOCUMENTATION)
