# Umicom Framework
# File: tests/sdk_documentation/check_layout.cmake
#
# PURPOSE:
# Verify component selection, canonical guide links and approved artwork in isolated install destinations.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

cmake_minimum_required(VERSION 3.24)
if(NOT DEFINED FRAMEWORK OR NOT DEFINED TEST_BINARY OR NOT DEFINED CASE OR
   NOT DEFINED TEST_GENERATOR)
    message(FATAL_ERROR "Supply FRAMEWORK, TEST_BINARY, CASE and TEST_GENERATOR")
endif()
set(_cases framework sdk docs default custom-datadir relocated missing-guide missing-brand)
if(NOT CASE IN_LIST _cases)
    message(FATAL_ERROR "Unknown documentation layout case")
endif()
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _id)
set(_root "${TEST_BINARY}/${CASE}-${_id}")
if(EXISTS "${_root}")
    message(FATAL_ERROR "Documentation fixture destination already exists")
endif()
set(_source "${_root}/source")
set(_build "${_root}/build")
set(_configured_prefix "${_root}/configured prefix")
set(_installed "${_configured_prefix}")
if(CASE STREQUAL "relocated")
    set(_installed "${_root}/relocated prefix")
endif()
set(_data share)
if(CASE STREQUAL "custom-datadir")
    set(_data "offline data")
endif()
file(MAKE_DIRECTORY "${_source}/docs/learning" "${_source}/resources/brand")
if(NOT CASE STREQUAL "missing-guide")
    file(MAKE_DIRECTORY "${_source}/docs/guides")
    file(WRITE "${_source}/docs/guides/view.html"
        "<a href=\"../learning/lesson.html\">Lesson</a><img src=\"../../resources/brand/umicom-logo.svg\" alt=\"Umicom\">")
endif()
file(WRITE "${_source}/docs/learning/lesson.html"
    "<a href=\"../guides/view.html\">Guide</a><link rel=\"icon\" href=\"../../resources/brand/umicom.ico\">")
set(_root_files DEVELOPMENT_WORKFLOW.html SELECTIVE_SDK_INSTALLATION.html
    KANBAN_PLANNING.html windows-installation.html WRITING_DOCUMENTATION.md)
foreach(_file IN LISTS _root_files)
    file(WRITE "${_source}/docs/${_file}" "Public documentation fixture: ${_file}\n")
endforeach()
set(_brand_files umicom-logo.svg umicom-logo-on-dark.svg umicom-icon.svg
    umicom-icon-on-dark.svg umicom.ico README.md)
foreach(_file IN LISTS _brand_files)
    if(CASE STREQUAL "missing-brand" AND _file STREQUAL "umicom-logo.svg")
        continue()
    endif()
    # Reuse exact approved masters; fixture art is never fabricated.
    file(COPY "${FRAMEWORK}/resources/brand/${_file}"
        DESTINATION "${_source}/resources/brand")
endforeach()
configure_file("${CMAKE_CURRENT_LIST_DIR}/fixture/CMakeLists.txt"
    "${_source}/CMakeLists.txt" COPYONLY)
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${_source}" -B "${_build}"
    -G "${TEST_GENERATOR}" "-DUMICOM_FRAMEWORK_SOURCE=${FRAMEWORK}"
    "-DCMAKE_INSTALL_PREFIX=${_configured_prefix}" "-DCMAKE_INSTALL_DATADIR=${_data}"
    RESULT_VARIABLE _configured OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 40)
file(WRITE "${_root}/configure.log" "${_out}\n${_err}")
if(CASE MATCHES "^missing-")
    if(_configured STREQUAL "0" OR NOT _err MATCHES "SDK documentation (brand )?input missing")
        message(FATAL_ERROR "Expected a missing documentation input refusal: ${_out}\n${_err}")
    endif()
    return()
endif()
if(NOT _configured STREQUAL "0")
    message(FATAL_ERROR "Documentation fixture configure failed: ${_out}\n${_err}")
endif()
set(_component)
if(CASE STREQUAL "framework")
    set(_component --component Framework)
elseif(CASE STREQUAL "sdk" OR CASE STREQUAL "custom-datadir")
    set(_component --component FrameworkSDK)
elseif(NOT CASE STREQUAL "default")
    set(_component --component FrameworkDocumentation)
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --install "${_build}"
    --prefix "${_installed}" ${_component}
    RESULT_VARIABLE _result OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 40)
file(WRITE "${_root}/install.log" "${_out}\n${_err}")
if(NOT _result STREQUAL "0")
    message(FATAL_ERROR "Documentation fixture install failed: ${_out}\n${_err}")
endif()
set(_canonical "${_installed}/${_data}/umicom/framework")
set(_expected docs/guides/view.html docs/learning/lesson.html)
foreach(_file IN LISTS _root_files)
    list(APPEND _expected "docs/${_file}")
endforeach()
foreach(_file IN LISTS _brand_files)
    list(APPEND _expected "resources/brand/${_file}")
endforeach()
foreach(_file IN LISTS _expected)
    if(NOT EXISTS "${_canonical}/${_file}" OR IS_DIRECTORY "${_canonical}/${_file}")
        message(FATAL_ERROR "Missing installed documentation file: ${_file}")
    endif()
    file(SHA256 "${_source}/${_file}" _source_hash)
    file(SHA256 "${_canonical}/${_file}" _installed_hash)
    if(NOT _source_hash STREQUAL _installed_hash)
        message(FATAL_ERROR "Installed documentation bytes changed: ${_file}")
    endif()
endforeach()
# These are the actual relative paths used by the fixtures. Check their final
# installed resolution, not merely a list of expected install destinations.
foreach(_linked
        "docs/guides/../learning/lesson.html"
        "docs/learning/../guides/view.html"
        "docs/guides/../../resources/brand/umicom-logo.svg"
        "docs/learning/../../resources/brand/umicom.ico")
    if(NOT EXISTS "${_canonical}/${_linked}")
        message(FATAL_ERROR "Installed relative documentation link is broken: ${_linked}")
    endif()
endforeach()
if(CASE STREQUAL "relocated" AND EXISTS "${_configured_prefix}")
    message(FATAL_ERROR "Relocated install wrote to its old configured prefix")
endif()
file(GLOB_RECURSE _installed_files LIST_DIRECTORIES false "${_installed}/*")
list(LENGTH _installed_files _actual_count)
list(LENGTH _expected _expected_count)
if(NOT _actual_count EQUAL _expected_count)
    message(FATAL_ERROR "Unexpected files in isolated documentation install")
endif()
