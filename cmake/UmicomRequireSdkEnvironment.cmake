#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomRequireSdkEnvironment.cmake
#
# PURPOSE:
#   Refuse an installed SDK with an incompatible declared build environment.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
# This is an early identity check, not binary compatibility certification.
# In particular equal pointer widths do not establish equal instruction sets,
# calling conventions, runtime-library models, struct packing or toolchain ABI.
function(umicom_require_sdk_environment expected_abi)
    if(NOT ARGC EQUAL 1 OR NOT expected_abi MATCHES "^[1-9][0-9]*$")
        message(FATAL_ERROR "SDK environment check requires one expected ABI identifier")
    endif()
    foreach(_name UMICOM_FRAMEWORK_ABI_VERSION UMICOM_FRAMEWORK_TARGET_SYSTEM
            UMICOM_FRAMEWORK_TARGET_POINTER_BYTES UMICOM_FRAMEWORK_PRODUCER_C_COMPILER
            UMICOM_FRAMEWORK_PRODUCER_C_COMPILER_VERSION)
        if(NOT DEFINED ${_name} OR "${${_name}}" STREQUAL "")
            message(FATAL_ERROR "SDK environment metadata is unavailable: ${_name}; install a current SDK")
        endif()
    endforeach()
    if(NOT DEFINED CMAKE_SYSTEM_NAME OR CMAKE_SYSTEM_NAME STREQUAL "" OR
       NOT DEFINED CMAKE_SIZEOF_VOID_P OR CMAKE_SIZEOF_VOID_P STREQUAL "" OR
       NOT DEFINED CMAKE_C_COMPILER_ID OR CMAKE_C_COMPILER_ID STREQUAL "")
        message(FATAL_ERROR "SDK environment check needs a configured C compiler; call it after project(... LANGUAGES C)")
    endif()
    if(NOT UMICOM_FRAMEWORK_TARGET_POINTER_BYTES MATCHES "^(2|4|8|16)$" OR
       NOT CMAKE_SIZEOF_VOID_P MATCHES "^(2|4|8|16)$")
        message(FATAL_ERROR "SDK environment pointer size is missing or unsupported")
    endif()
    if(NOT "${UMICOM_FRAMEWORK_ABI_VERSION}" STREQUAL "${expected_abi}")
        message(FATAL_ERROR
            "SDK ABI ${UMICOM_FRAMEWORK_ABI_VERSION} differs from required ABI ${expected_abi}")
    endif()
    if(NOT "${UMICOM_FRAMEWORK_TARGET_SYSTEM}" STREQUAL "${CMAKE_SYSTEM_NAME}")
        message(FATAL_ERROR
            "SDK target system ${UMICOM_FRAMEWORK_TARGET_SYSTEM} differs from consumer ${CMAKE_SYSTEM_NAME}")
    endif()
    if(NOT "${UMICOM_FRAMEWORK_TARGET_POINTER_BYTES}" STREQUAL "${CMAKE_SIZEOF_VOID_P}")
        message(FATAL_ERROR
            "SDK pointer size ${UMICOM_FRAMEWORK_TARGET_POINTER_BYTES} differs from consumer ${CMAKE_SIZEOF_VOID_P}")
    endif()
    if(NOT "${UMICOM_FRAMEWORK_PRODUCER_C_COMPILER}" STREQUAL "${CMAKE_C_COMPILER_ID}")
        message(FATAL_ERROR
            "SDK compiler family ${UMICOM_FRAMEWORK_PRODUCER_C_COMPILER} differs from consumer ${CMAKE_C_COMPILER_ID}; qualify a matching SDK explicitly")
    endif()
    # Version is reported rather than assumed compatible or incompatible.
    # Some releases preserve a binary interface; others change it through flags
    # or runtime selection. Consumer compile/link/runtime checks remain required.
    message(STATUS "Framework SDK environment identifiers agree: ABI ${expected_abi}, ${CMAKE_SYSTEM_NAME}, ${CMAKE_SIZEOF_VOID_P}-byte pointers")
    message(STATUS "Framework SDK producer: ${UMICOM_FRAMEWORK_PRODUCER_C_COMPILER} ${UMICOM_FRAMEWORK_PRODUCER_C_COMPILER_VERSION}; consumer: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
endfunction()
