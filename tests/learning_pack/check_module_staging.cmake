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
    set(stage_error "${_err}" PARENT_SCOPE)
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

# A changed early resource must not reach disk while later resources have stale
# digests. Preserve both the existing staged package and the publisher's source
# manifest: the build cannot decide whether changed lesson content is approved.
file(SHA256 "${_output}/lessons/intro.md" _previous_lesson)
file(SHA256 "${_output}/learning-pack.json" _previous_manifest)
file(WRITE "${_source}/lessons/second.md" "# Second lesson\nInitial content.\n")
file(SHA256 "${_source}/lessons/second.md" _second_old)
file(WRITE "${_source}/lessons/third.md" "# Third lesson\nInitial content.\n")
file(SHA256 "${_source}/lessons/third.md" _third_old)
file(WRITE "${_source}/lessons/intro.md" "# Introduction\nReviewed updated explanation.\n")
file(SHA256 "${_source}/lessons/intro.md" _hash)
file(WRITE "${_source}/lessons/second.md" "# Second lesson\nChanged content.\n")
file(WRITE "${_source}/lessons/third.md" "# Third lesson\nChanged content.\n")
function(write_fixture_manifest second_hash third_hash)
    file(WRITE "${_source}/learning-pack.json"
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"stage.fixture\",\"product\":\"studio\","
        "\"resources\":[{\"path\":\"lessons/intro.md\",\"sha256\":\"${_hash}\"},"
        "{\"path\":\"lessons/second.md\",\"sha256\":\"${second_hash}\"},"
        "{\"path\":\"lessons/third.md\",\"sha256\":\"${third_hash}\"}]}\n")
endfunction()
write_fixture_manifest("${_second_old}" "${_third_old}")
file(SHA256 "${_source}/learning-pack.json" _stale_manifest)
set(_rejected "${_root}/rejected parent/new module")
stage("${_rejected}" failure "Learning resource checksum mismatch: 2 resource")
foreach(_detail IN ITEMS "lessons/second.md" "lessons/third.md" "Manifest:" "learning-pack.json")
    string(FIND "${stage_error}" "${_detail}" _found)
    if(_found LESS 0)
        message(FATAL_ERROR "Checksum rejection omitted affected resource or manifest: ${_detail}")
    endif()
endforeach()
if(EXISTS "${_root}/rejected parent")
    message(FATAL_ERROR "Checksum rejection created a destination directory")
endif()
stage("${_output}" failure "Learning resource checksum mismatch: 2 resource")
file(SHA256 "${_output}/lessons/intro.md" _after_lesson)
file(SHA256 "${_output}/learning-pack.json" _after_manifest)
file(SHA256 "${_source}/learning-pack.json" _source_after)
if(NOT _previous_lesson STREQUAL _after_lesson OR
   NOT _previous_manifest STREQUAL _after_manifest OR
   NOT _stale_manifest STREQUAL _source_after OR
   EXISTS "${_output}/lessons/second.md" OR EXISTS "${_output}/lessons/third.md")
    message(FATAL_ERROR "Checksum rejection changed source or partially replaced the staged module")
endif()
# After the publisher reviews the content, a matching manifest permits staging.
# Validate-only remains a read-only operation even with an explicit destination.
file(SHA256 "${_source}/lessons/second.md" _second_hash)
file(SHA256 "${_source}/lessons/third.md" _third_hash)
write_fixture_manifest("${_second_hash}" "${_third_hash}")
execute_process(COMMAND "${CMAKE_COMMAND}"
    "-DUMI_MODULE_SOURCE=${_source}" "-DUMI_MODULE_DESTINATION=${_rejected}"
    -DUMI_MODULE_ID=stage.fixture -DUMI_MODULE_PRODUCT=studio
    -DUMI_MODULE_VALIDATE_ONLY=ON -P "${UMI_STAGE_SCRIPT}"
    RESULT_VARIABLE _validate_result OUTPUT_VARIABLE _validate_out ERROR_VARIABLE _validate_err)
if(NOT _validate_result EQUAL 0 OR "${_validate_out}${_validate_err}" MATCHES "[Ww]arning" OR
   EXISTS "${_root}/rejected parent")
    message(FATAL_ERROR "Validate-only failed or wrote files: ${_validate_out}\n${_validate_err}")
endif()
stage("${_output}" success "")
foreach(_relative IN ITEMS "learning-pack.json" "lessons/second.md" "lessons/third.md")
    file(SHA256 "${_source}/${_relative}" _source_hash)
    file(SHA256 "${_output}/${_relative}" _staged_hash)
    if(NOT _source_hash STREQUAL _staged_hash)
        message(FATAL_ERROR "Reviewed module did not stage complete content: ${_relative}")
    endif()
endforeach()

# A valid filename may also be a CMake false constant. Count mismatches instead
# of interpreting a filename as a boolean, or a single OFF resource could pass.
file(WRITE "${_source}/OFF" "Initial content.\n")
file(SHA256 "${_source}/OFF" _false_name_hash)
file(WRITE "${_source}/learning-pack.json"
    "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"stage.fixture\",\"product\":\"studio\","
    "\"resources\":[{\"path\":\"OFF\",\"sha256\":\"${_false_name_hash}\"}]}\n")
file(WRITE "${_source}/OFF" "Changed content.\n")
stage("${_rejected}" failure "Learning resource checksum mismatch: 1 resource")
if(EXISTS "${_root}/rejected parent")
    message(FATAL_ERROR "A false-like filename bypassed checksum preflight")
endif()

message(STATUS "Module staging fixture: ${_root}")
