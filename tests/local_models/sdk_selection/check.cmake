#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/local_models/sdk_selection/check.cmake
#
# PURPOSE:
#   Exercise actual consumer package selection, stale caches and incompatible installed metadata.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.24)
foreach(_required IN ITEMS FRAMEWORK OUTPUT_ROOT GENERATOR C_COMPILER CASE)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "Model SDK selection needs ${_required}")
    endif()
endforeach()
if(NOT IS_ABSOLUTE "${FRAMEWORK}" OR NOT IS_ABSOLUTE "${OUTPUT_ROOT}")
    message(FATAL_ERROR "Model SDK fixture requires absolute source and output roots")
endif()
set(_cases explicit discovered spaces cached-other switch-root relative missing-root
    missing-component missing-guard wrong-prefix changed-header extra-header
    outside-library missing-archive abi compiler metadata)
if(NOT CASE IN_LIST _cases)
    message(FATAL_ERROR "Unknown model SDK selection scenario")
endif()
include("${FRAMEWORK}/cmake/UmicomPublicContractFingerprint.cmake")
# Never erase prior test evidence. Each invocation gets isolated SDKs and logs;
# negative scenarios repeat in the same consumer build to expose cache residue.
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _run)
set(_work "${OUTPUT_ROOT}/run-${_run}")
file(MAKE_DIRECTORY "${_work}")
set(OUTSIDE_ARCHIVE "${_work}/outside.a")
file(WRITE "${OUTSIDE_ARCHIVE}" "configuration fixture only\n")
function(_fixture_sdk prefix)
    file(MAKE_DIRECTORY "${prefix}/include/umicom/base" "${prefix}/lib/cmake/UmicomFramework")
    file(WRITE "${prefix}/include/umicom/base/version.h" "/* Configure-only ABI fixture. */\n#define FIXTURE_ABI 2\n")
    file(WRITE "${prefix}/lib/base.a" "configuration fixture only\n")
    umicom_compute_public_contract_fingerprint(FIXTURE_FINGERPRINT "${prefix}/include")
    foreach(_helper IN ITEMS UmicomPublicContractFingerprint.cmake
            UmicomRequireInstalledSdk.cmake UmicomRequireSdkEnvironment.cmake)
        configure_file("${FRAMEWORK}/cmake/${_helper}"
            "${prefix}/lib/cmake/UmicomFramework/${_helper}" COPYONLY)
    endforeach()
    configure_file("${CMAKE_CURRENT_LIST_DIR}/UmicomFrameworkConfig.cmake.in"
        "${prefix}/lib/cmake/UmicomFramework/UmicomFrameworkConfig.cmake" @ONLY)
endfunction()
set(_sdk "${_work}/sdk")
if(CASE STREQUAL "spaces")
    set(_sdk "${_work}/SDK with spaces")
endif()
_fixture_sdk("${_sdk}")
set(_other "${_work}/other-sdk")
_fixture_sdk("${_other}")
if(CASE STREQUAL "changed-header")
    file(APPEND "${_sdk}/include/umicom/base/version.h" "#define MIXED_INPUT 1\n")
elseif(CASE STREQUAL "extra-header")
    file(WRITE "${_sdk}/include/umicom/stale.h" "/* Unexpected installed header. */\n")
endif()
set(_expected "")
if(CASE MATCHES "^(cached-other|switch-root|wrong-prefix)$")
    set(_expected "not from the requested SDK prefix")
elseif(CASE MATCHES "^(relative|missing-root)$")
    set(_expected "must be an absolute existing SDK prefix")
elseif(CASE STREQUAL "missing-component")
    set(_expected "local_models component unavailable")
elseif(CASE STREQUAL "missing-guard")
    set(_expected "requires installed SDK contract helpers")
elseif(CASE MATCHES "^(changed-header|extra-header)$")
    set(_expected "Installed headers differ")
elseif(CASE STREQUAL "outside-library")
    set(_expected "outside the selected installed SDK")
elseif(CASE STREQUAL "missing-archive")
    set(_expected "missing or not absolute")
elseif(CASE STREQUAL "abi")
    set(_expected "differs from required ABI")
elseif(CASE STREQUAL "compiler")
    set(_expected "SDK compiler family")
elseif(CASE STREQUAL "metadata")
    set(_expected "SDK environment metadata is unavailable")
endif()
set(_command "${CMAKE_COMMAND}" -S "${FRAMEWORK}/examples/local_models/sdk_consumer"
    -B "${_work}/build" -G "${GENERATOR}" "-DCMAKE_C_COMPILER:FILEPATH=${C_COMPILER}"
    -DCMAKE_FIND_USE_PACKAGE_REGISTRY=OFF -DCMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY=OFF)
if(DEFINED MAKE_PROGRAM AND NOT MAKE_PROGRAM STREQUAL "")
    list(APPEND _command "-DCMAKE_MAKE_PROGRAM:FILEPATH=${MAKE_PROGRAM}")
endif()
if(DEFINED TOOLCHAIN AND NOT TOOLCHAIN STREQUAL "")
    list(APPEND _command "-DCMAKE_TOOLCHAIN_FILE:FILEPATH=${TOOLCHAIN}")
endif()
set(_attempts 1)
if(NOT _expected STREQUAL "")
    set(_attempts 2)
endif()
if(CASE STREQUAL "switch-root")
    set(_attempts 3)
endif()
foreach(_attempt RANGE 1 ${_attempts})
    set(_arguments "-DUMICOM_SDK_ROOT:STRING=${_sdk}" "-DSDK_CASE=${CASE}")
    set(_want_error "${_expected}")
    if(CASE STREQUAL "discovered")
        set(_arguments -DUMICOM_SDK_ROOT:STRING= "-DCMAKE_PREFIX_PATH:PATH=${_sdk}" "-DSDK_CASE=${CASE}")
    elseif(CASE STREQUAL "relative")
        set(_arguments -DUMICOM_SDK_ROOT:STRING=relative-sdk "-DSDK_CASE=${CASE}")
    elseif(CASE STREQUAL "missing-root")
        set(_arguments "-DUMICOM_SDK_ROOT:STRING=${_work}/absent" "-DSDK_CASE=${CASE}")
    elseif(CASE STREQUAL "cached-other")
        list(APPEND _arguments "-DUmicomFramework_DIR:PATH=${_other}/lib/cmake/UmicomFramework")
    elseif(CASE STREQUAL "switch-root")
        if(_attempt EQUAL 1)
            set(_want_error "")
        else()
            set(_arguments "-DUMICOM_SDK_ROOT:STRING=${_other}" "-DSDK_CASE=${CASE}")
        endif()
    endif()
    execute_process(COMMAND ${_command} ${_arguments}
        RESULT_VARIABLE _result OUTPUT_VARIABLE _out ERROR_VARIABLE _err
        ENCODING UTF-8 TIMEOUT 70)
    file(WRITE "${_work}/configure-${_attempt}.log" "${_out}\n${_err}")
    file(WRITE "${_work}/configure-${_attempt}.exit.txt" "${_result}\n")
    string(REGEX REPLACE "[ \t\r\n]+" " " _diagnostic "${_out}\n${_err}")
    if(_want_error STREQUAL "")
        if(NOT _result STREQUAL "0" OR
           NOT _out MATCHES "Model consumer selected installed SDK:" OR
           NOT EXISTS "${_work}/build/fixture-admitted.txt")
            message(FATAL_ERROR "Valid model SDK fixture refused; see ${_work}/configure-${_attempt}.log")
        endif()
        file(STRINGS "${_work}/build/fixture-admitted.txt" _admitted)
        if(NOT _admitted STREQUAL _sdk)
            message(FATAL_ERROR "Consumer admitted an unexpected SDK prefix")
        endif()
        if(CASE STREQUAL "discovered" AND NOT _out MATCHES "auto-discovered Framework SDK:")
            message(FATAL_ERROR "Discovery was incorrectly described as explicit selection")
        endif()
    elseif(_result STREQUAL "0" OR NOT _diagnostic MATCHES "${_want_error}")
        message(FATAL_ERROR "Expected SDK refusal missing; see ${_work}/configure-${_attempt}.log")
    endif()
endforeach()
message(STATUS "Model SDK selection fixture checked: ${CASE}; no libraries built or applications run")
