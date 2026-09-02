#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLivingDocumentation.cmake
#
# PURPOSE:
#   Create an optional HTML code-reference target from structured comments in
#   Framework and application source files without storing generated pages in
#   the source repository.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)

option(UMICOM_BUILD_LIVING_DOCUMENTATION
       "Provide the umicom-documentation HTML reference target"
       ON)

set(UMICOM_DOCUMENTATION_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/documentation"
    CACHE PATH
    "Folder used for generated Umicom HTML documentation")

# Disabling this option removes all discovery and target registration, which
# is useful for minimal toolchains that never publish local documentation.
if(UMICOM_BUILD_LIVING_DOCUMENTATION)
    find_package(Doxygen QUIET)

    # HTML generation is registered only after CMake has found a compatible
    # executable; normal configure and build paths stay independent from it.
    if(DOXYGEN_FOUND)
        set(_umicom_documentation_roots
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/src"
            "${CMAKE_CURRENT_SOURCE_DIR}/docs")

        # A suite build includes thin application contracts as well as
        # Framework, so one reference explains both sides of each boundary.
        if(EXISTS "${CMAKE_SOURCE_DIR}/applications")
            list(APPEND _umicom_documentation_roots
                "${CMAKE_SOURCE_DIR}/applications")
        endif()

        set(UMICOM_DOCUMENTATION_INPUT "")
        # Quote every path separately so spaces in a developer's workspace do
        # not split one documentation input into several invalid entries.
        foreach(_umicom_documentation_root IN LISTS
                _umicom_documentation_roots)
            string(APPEND UMICOM_DOCUMENTATION_INPUT
                " \"${_umicom_documentation_root}\"")
        endforeach()

        set(UMICOM_DOCUMENTATION_MAIN_PAGE
            "${CMAKE_CURRENT_SOURCE_DIR}/docs/LIVING_CODE_DOCUMENTATION.md")
        set(UMICOM_DOCUMENTATION_STYLESHEET
            "${CMAKE_CURRENT_SOURCE_DIR}/docs/documentation.css")

        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in"
            "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
            @ONLY)

        # Build one glob pattern per source root. Appending a suffix directly
        # to a CMake list would otherwise apply it to only the last entry.
        set(_umicom_documentation_patterns)
        # Each root contributes all three documented source formats.
        foreach(_umicom_documentation_root IN LISTS
                _umicom_documentation_roots)
            list(APPEND _umicom_documentation_patterns
                "${_umicom_documentation_root}/*.c"
                "${_umicom_documentation_root}/*.h"
                "${_umicom_documentation_root}/*.md")
        endforeach()

        # Source dependencies make the target incremental. Adding or changing
        # a C, header or Markdown file refreshes the affected reference pages.
        file(GLOB_RECURSE _umicom_documentation_sources
             CONFIGURE_DEPENDS
             LIST_DIRECTORIES FALSE
             ${_umicom_documentation_patterns})

        add_custom_command(
            OUTPUT
                "${UMICOM_DOCUMENTATION_OUTPUT_DIRECTORY}/html/index.html"
            COMMAND
                "${CMAKE_COMMAND}" -E make_directory
                "${UMICOM_DOCUMENTATION_OUTPUT_DIRECTORY}"
            COMMAND
                "${DOXYGEN_EXECUTABLE}"
                "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
            DEPENDS
                ${_umicom_documentation_sources}
                "${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in"
                "${UMICOM_DOCUMENTATION_MAIN_PAGE}"
                "${UMICOM_DOCUMENTATION_STYLESHEET}"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            COMMENT "Generating the navigable Umicom code reference"
            VERBATIM)

        add_custom_target(umicom-documentation
            DEPENDS
                "${UMICOM_DOCUMENTATION_OUTPUT_DIRECTORY}/html/index.html")

        set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES
            "${UMICOM_DOCUMENTATION_OUTPUT_DIRECTORY}")
    # Use this fallback path when the earlier condition does not apply.
    else()
        # A missing optional generator must not block normal application work.
        message(STATUS
            "Umicom HTML documentation is available after a compatible "
            "source-documentation generator is installed.")
    endif()
endif()
