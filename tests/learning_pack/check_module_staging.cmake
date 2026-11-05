#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/learning_pack/check_module_staging.cmake
#
# PURPOSE:
#   Verify first-use module staging and reject unsafe output paths before copying.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#---------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.26)
if(NOT IS_ABSOLUTE "${UMI_STAGE_TEST_ROOT}" OR NOT IS_ABSOLUTE "${UMI_STAGE_SCRIPT}")
    message(FATAL_ERROR "Explicit absolute fixture and staging paths are required")
endif()
# Each invocation owns a fresh folder, including repeated owner qualification.
# Retain fixture output for diagnosis instead of deleting earlier evidence.
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _run)
set(_root "${UMI_STAGE_TEST_ROOT}/${_run}")
set(_source "${_root}/source")
file(MAKE_DIRECTORY "${_source}/lessons")
file(WRITE "${_source}/lessons/intro.md" "# Introduction\nRead a local lesson.\n")
file(SHA256 "${_source}/lessons/intro.md" _hash)
file(WRITE "${_source}/learning-pack.json"
    "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"stage.fixture\",\"product\":\"studio\","
    "\"resources\":[{\"path\":\"lessons/intro.md\",\"sha256\":\"${_hash}\"}]}\n")

function(stage destination expected diagnostic)
    execute_process(COMMAND "${CMAKE_COMMAND}"
        "-DUMI_MODULE_SOURCE=${_source}" "-DUMI_MODULE_DESTINATION=${destination}"
        -DUMI_MODULE_ID=stage.fixture -DUMI_MODULE_PRODUCT=studio
        -P "${UMI_STAGE_SCRIPT}"
        RESULT_VARIABLE _result OUTPUT_VARIABLE _out ERROR_VARIABLE _err)
    if(expected STREQUAL "success")
        if(NOT _result EQUAL 0 OR "${_out}${_err}" MATCHES "[Ww]arning")
            message(FATAL_ERROR "Staging must succeed without warnings: ${_result}\n${_out}\n${_err}")
        endif()
        file(SHA256 "${destination}/lessons/intro.md" _copied)
        if(NOT _copied STREQUAL _hash)
            message(FATAL_ERROR "Staged lesson differs from its declared source")
        endif()
    elseif(_result EQUAL 0 OR NOT "${_err}" MATCHES "${diagnostic}")
        message(FATAL_ERROR "Expected staging rejection: ${diagnostic}\n${_out}\n${_err}")
    endif()
endfunction()

set(_output "${_root}/new parent/bin/learning-modules/fixture")
if(EXISTS "${_output}")
    message(FATAL_ERROR "First-use fixture must have a missing destination")
endif()
stage("${_output}" success "")
# Existing output must also remain valid and unrelated files must survive.
file(WRITE "${_output}/owner-note.txt" "Retain this unrelated file.\n")
stage("${_output}" success "")
file(READ "${_output}/owner-note.txt" _retained)
if(NOT _retained STREQUAL "Retain this unrelated file.\n")
    message(FATAL_ERROR "Repeat staging changed an unrelated file")
endif()
stage("${_source}/new/output" failure "must not overlap")
if(EXISTS "${_source}/new")
    message(FATAL_ERROR "Rejected output created directories in the source")
endif()
stage("${_root}" failure "must not overlap")
if(EXISTS "${_root}/learning-pack.json")
    message(FATAL_ERROR "Rejected ancestor destination received a manifest")
endif()
file(WRITE "${_root}/blocked" "Keep this file.\n")
stage("${_root}/blocked/child" failure "not a directory")
file(READ "${_root}/blocked" _blocked)
if(NOT _blocked STREQUAL "Keep this file.\n")
    message(FATAL_ERROR "Rejected staging changed an existing ancestor file")
endif()
if(WIN32)
    string(TOUPPER "${_source}" _other_case)
    stage("${_other_case}/case-output" failure "must not overlap")
endif()
message(STATUS "Module staging fixture: ${_root}")
