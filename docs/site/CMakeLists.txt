#-----------------------------------------------------------------------------
# Umicom Framework
# File: docs/site/CMakeLists.txt
# PURPOSE: Assemble public guides and source locations using the existing
#          Doxygen configuration, without compiling product executables.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomDocumentationSite LANGUAGES NONE)
include(CTest)
find_package(Git REQUIRED)

get_filename_component(UMICOM_SITE_FRAMEWORK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
get_filename_component(_suite_default "${UMICOM_SITE_FRAMEWORK_ROOT}/.." ABSOLUTE)
set(UMICOM_SITE_APPLICATIONS_ROOT "${_suite_default}" CACHE PATH "Complete Applications checkout")
set(UMICOM_SITE_SOURCE_REF "main" CACHE STRING "Framework source-link branch or 40-character commit")
if(NOT UMICOM_SITE_SOURCE_REF MATCHES "^[A-Za-z0-9_.-]+$")
    message(FATAL_ERROR "Source reference must be a simple branch name or commit hash, not a URL.")
endif()
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    message(FATAL_ERROR "Use a separate build directory, for example -B build/documentation.")
endif()
if(NOT EXISTS "${UMICOM_SITE_APPLICATIONS_ROOT}/.gitmodules")
    message(FATAL_ERROR "A full Applications checkout is required. Set UMICOM_SITE_APPLICATIONS_ROOT to it.")
endif()
set(UMICOM_SITE_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/site")
file(MAKE_DIRECTORY "${UMICOM_SITE_OUTPUT}")

# HTML examples use the real source file, not a hand-maintained website copy.
function(UmiSiteEscape input output)
    string(REPLACE "&" "&amp;" _text "${input}")
    string(REPLACE "<" "&lt;" _text "${_text}")
    string(REPLACE ">" "&gt;" _text "${_text}")
    string(REPLACE "\"" "&quot;" _text "${_text}")
    set(${output} "${_text}" PARENT_SCOPE)
endfunction()
set(_diagnostic_example "${UMICOM_SITE_FRAMEWORK_ROOT}/examples/build_diagnostics/main.c")
set(_document_example "${UMICOM_SITE_FRAMEWORK_ROOT}/examples/editor_workflow/complete_document.c")
set(_window_example "${UMICOM_SITE_FRAMEWORK_ROOT}/examples/learning/projects/notes_window.c")
foreach(_file IN ITEMS "${_diagnostic_example}" "${_document_example}" "${_window_example}")
    if(NOT EXISTS "${_file}")
        message(FATAL_ERROR "Required existing example is missing: ${_file}")
    endif()
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_file}")
endforeach()
file(READ "${_diagnostic_example}" _example)
UmiSiteEscape("${_example}" UMICOM_SITE_DIAGNOSTIC_EXAMPLE)
file(READ "${_document_example}" _example)
UmiSiteEscape("${_example}" UMICOM_SITE_DOCUMENT_EXAMPLE)
file(READ "${_window_example}" _window)
# The fragment is bounded by existing function names and must be present in full.
string(FIND "${_window}" "static GtkWidget *Component(" _start)
string(FIND "${_window}" "static void DestroyState(" _end)
if(_start LESS 0 OR _end LESS _start)
    message(FATAL_ERROR "The Notes component helper changed. Review its Wiki excerpt.")
endif()
math(EXPR _length "${_end} - ${_start}")
string(SUBSTRING "${_window}" ${_start} ${_length} _example)
UmiSiteEscape("${_example}" UMICOM_SITE_COMPONENT_EXAMPLE)

# This is a file inventory, not a second C parser or a runtime module registry.
file(GLOB_RECURSE _headers CONFIGURE_DEPENDS LIST_DIRECTORIES FALSE
    RELATIVE "${UMICOM_SITE_FRAMEWORK_ROOT}" "${UMICOM_SITE_FRAMEWORK_ROOT}/include/umicom/*.h")
list(SORT _headers)
list(LENGTH _headers UMICOM_SITE_HEADER_COUNT)
set(_families)
set(UMICOM_SITE_HEADER_ROWS "")
foreach(_header IN LISTS _headers)
    # Source paths enter HTML attributes. Refuse unexpected filenames instead of
    # constructing an ambiguous or unsafe URL from arbitrary text.
    if(NOT _header MATCHES "^include/umicom/[A-Za-z0-9_./-]+\\.h$")
        message(FATAL_ERROR "Review the public-header URL encoding for: ${_header}")
    endif()
    string(REGEX REPLACE "^include/umicom/([^/]+)/.*$" "\\1" _family "${_header}")
    if(_family STREQUAL _header)
        set(_family "root")
    endif()
    list(APPEND _families "${_family}")
    string(APPEND UMICOM_SITE_HEADER_ROWS
        "<tr data-source-row><td>${_family}</td><td><a href=\"https://github.com/umicom-foundation/umicom-framework/blob/${UMICOM_SITE_SOURCE_REF}/${_header}\"><code>${_header}</code></a></td></tr>\n")
endforeach()
list(REMOVE_DUPLICATES _families)
list(LENGTH _families UMICOM_SITE_FAMILY_COUNT)
if(UMICOM_SITE_HEADER_COUNT EQUAL 0)
    message(FATAL_ERROR "No public Framework headers were found.")
endif()
configure_file(templates/source-map.html.in "${UMICOM_SITE_OUTPUT}/source-map.html" @ONLY)

file(GLOB_RECURSE _pages CONFIGURE_DEPENDS LIST_DIRECTORIES FALSE
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/pages" "${CMAKE_CURRENT_SOURCE_DIR}/pages/*")
foreach(_page IN LISTS _pages)
    string(REGEX REPLACE "\\.in$" "" _destination "${_page}")
    # Do not replace a previously generated API start page with the help page
    # when CMake reconfigures. The API target owns its generated directory.
    if(_page STREQUAL "reference/html/index.html" AND EXISTS "${UMICOM_SITE_OUTPUT}/${_destination}")
        continue()
    endif()
    configure_file("pages/${_page}" "${UMICOM_SITE_OUTPUT}/${_destination}" @ONLY)
endforeach()

set(_product_names studio trader desktop bank)
foreach(_product IN LISTS _product_names)
    set(_page "${UMICOM_SITE_APPLICATIONS_ROOT}/applications/${_product}/docs/site/index.html")
    if(NOT EXISTS "${_page}")
        message(FATAL_ERROR "Merge the ${_product} public site page before building the combined site.")
    endif()
    configure_file("${_page}" "${UMICOM_SITE_OUTPUT}/${_product}/index.html" COPYONLY)
endforeach()
# Reuse established tutorials and the full build/run/install/Git guide in output.
configure_file("${UMICOM_SITE_APPLICATIONS_ROOT}/applications/studio/docs/EDITING_FILES.html"
    "${UMICOM_SITE_OUTPUT}/studio/EDITING_FILES.html" COPYONLY)
configure_file("${UMICOM_SITE_APPLICATIONS_ROOT}/applications/studio/docs/READING_BUILD_ERRORS.html"
    "${UMICOM_SITE_OUTPUT}/studio/READING_BUILD_ERRORS.html" COPYONLY)
configure_file("${UMICOM_SITE_APPLICATIONS_ROOT}/docs/getting-started/BUILDING_STUDIO.html"
    "${UMICOM_SITE_OUTPUT}/build-and-publish.html" COPYONLY)
file(WRITE "${UMICOM_SITE_OUTPUT}/.nojekyll" "")
add_custom_target(umicom-website
    COMMAND "${CMAKE_COMMAND}" -E echo "Public site: ${UMICOM_SITE_OUTPUT}/index.html"
    VERBATIM)

# Reuse the established Doxyfile template, but choose only source and public
# reference content. Historical reports/private working documents are not inputs.
set(UMICOM_DOCUMENTATION_OUTPUT_DIRECTORY "${UMICOM_SITE_OUTPUT}/reference")
set(UMICOM_DOCUMENTATION_MAIN_PAGE "${CMAKE_CURRENT_SOURCE_DIR}/templates/reference.md")
set(UMICOM_DOCUMENTATION_STYLESHEET "${UMICOM_SITE_FRAMEWORK_ROOT}/docs/documentation.css")
set(_reference_roots "${UMICOM_SITE_FRAMEWORK_ROOT}/include" "${UMICOM_SITE_FRAMEWORK_ROOT}/src"
    "${UMICOM_SITE_FRAMEWORK_ROOT}/adapters" "${UMICOM_DOCUMENTATION_MAIN_PAGE}")
file(GLOB _application_directories LIST_DIRECTORIES TRUE "${UMICOM_SITE_APPLICATIONS_ROOT}/applications/*")
foreach(_application IN LISTS _application_directories)
    foreach(_kind IN ITEMS include src)
        if(IS_DIRECTORY "${_application}/${_kind}")
            list(APPEND _reference_roots "${_application}/${_kind}")
        endif()
    endforeach()
endforeach()
set(UMICOM_DOCUMENTATION_INPUT "")
foreach(_input IN LISTS _reference_roots)
    string(APPEND UMICOM_DOCUMENTATION_INPUT " \"${_input}\"")
endforeach()
configure_file("${UMICOM_SITE_FRAMEWORK_ROOT}/docs/Doxyfile.in"
    "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    add_custom_target(umicom-website-api
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${UMICOM_DOCUMENTATION_OUTPUT_DIRECTORY}"
        COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        WORKING_DIRECTORY "${UMICOM_SITE_APPLICATIONS_ROOT}"
        COMMENT "Generating the public source reference with Doxygen"
        VERBATIM)
else()
    message(STATUS "Doxygen is unavailable. The public pages build; the API target will report this requirement.")
    add_custom_target(umicom-website-api
        COMMAND "${CMAKE_COMMAND}" -E echo "Install Doxygen and reconfigure before generating the API reference."
        COMMAND "${CMAKE_COMMAND}" -E false
        VERBATIM)
endif()
add_dependencies(umicom-website-api umicom-website)
add_custom_target(umicom-website-package
    COMMAND "${CMAKE_COMMAND}" -E tar cf "${CMAKE_CURRENT_BINARY_DIR}/umicom-website.zip" --format=zip site
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS umicom-website
    COMMENT "Packaging the generated public website (build the API target first when needed)"
    VERBATIM)

if(BUILD_TESTING)
    foreach(_case IN ITEMS pages catalogue examples exclusions products)
        add_test(NAME "documentation.site.${_case}"
            COMMAND "${CMAKE_COMMAND}"
                "-DSITE=${UMICOM_SITE_OUTPUT}"
                "-DFRAMEWORK=${UMICOM_SITE_FRAMEWORK_ROOT}"
                "-DEXPECTED_HEADERS=${UMICOM_SITE_HEADER_COUNT}"
                "-DSUITE=${UMICOM_SITE_APPLICATIONS_ROOT}"
                "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}"
                "-DDOXYFILE=${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
                "-DCASE=${_case}" -P "${CMAKE_CURRENT_SOURCE_DIR}/check_site.cmake")
    endforeach()
endif()
message(STATUS "Public source map: ${UMICOM_SITE_HEADER_COUNT} headers in ${UMICOM_SITE_FAMILY_COUNT} source families")
