#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomValueOwnerBuildChecks.cmake
# PURPOSE: Keep value-record consumers and their real libraries on matching implementations.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomImplementationFingerprint.cmake")
set(_umicom_value_root "${CMAKE_CURRENT_LIST_DIR}/..")

# These libraries own checked value constructors or paged registry reads.
# Append an owner here when adding another public value family, and extend its
# symbol manifest with the actual exported functions. Tests still link against
# the real libraries; no fallback implementation is provided by a test.
set(_umicom_value_owners
    umicom_chart
    umicom_debug
    umicom_designer
    umicom_desktop
    umicom_editor
    umicom_frontend
    umicom_language
    umicom_platform
    umicom_product
    umicom_project
    umicom_sdk_runtime
    umicom_source_control
    umicom_test_platform
    umicom_test_runtime
    umicom_ui
)

# The former common-input registration is retained for review. Private
# helpers now follow their actual including sources, so an unrelated object
# does not rebuild merely because another record family uses a changed helper.
if(FALSE)
foreach(_owner IN LISTS _umicom_value_owners)
    umicom_attach_implementation_fingerprint("${_owner}" "${_umicom_value_root}"
        src/base/record_update_internal.h src/base/snapshot_registry_internal.h)
endforeach()
endif()
foreach(_owner IN LISTS _umicom_value_owners)
    umicom_attach_implementation_fingerprint("${_owner}" "${_umicom_value_root}")
endforeach()
# CTest's included parsers and queued runner also need content identities when
# full-file deliveries retain dates from an earlier checkout.
umicom_attach_implementation_fingerprint(umicom_testing "${_umicom_value_root}")

# Reuse the archive verifier's checks for absent, duplicate or non-code symbols.
# Run on every normal build rather than trusting an earlier success report.
# Toolchains without GNU/LLVM nm retain the executable link regressions.
if(CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$" AND NOT MSVC)
    set(_umicom_value_underscore OFF)
    if(APPLE OR (WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 4))
        set(_umicom_value_underscore ON)
    endif()
    add_custom_target(umicom-value-link-checks ALL)
    foreach(_owner IN LISTS _umicom_value_owners)
        set(_check "${_owner}-value-symbol-check")
        set(_manifest "${CMAKE_CURRENT_LIST_DIR}/link_contracts/${_owner}_values.symbols")
        add_custom_target("${_check}"
            COMMAND "${CMAKE_COMMAND}"
                "-DARCHIVE=$<TARGET_FILE:${_owner}>"
                "-DSYMBOL_FILE=${_manifest}"
                "-DNM_TOOL=${CMAKE_NM}"
                "-DALLOW_LEADING_UNDERSCORE=${_umicom_value_underscore}"
                "-DREPORT=${CMAKE_BINARY_DIR}/link-reports/$<CONFIG>/${_owner}-values.json"
                -P "${CMAKE_CURRENT_LIST_DIR}/VerifyLibrarySymbols.cmake"
            COMMENT "Verify ${_owner} value function definitions" VERBATIM)
        add_dependencies("${_check}" "${_owner}")
        add_dependencies(umicom-value-link-checks "${_check}")
        if(BUILD_TESTING)
            # The test uses a separate report so CTest and a build cannot write
            # the same evidence file concurrently.
            add_test(NAME "framework.value_link.archive.${_owner}"
                COMMAND "${CMAKE_COMMAND}"
                    "-DARCHIVE=$<TARGET_FILE:${_owner}>" "-DSYMBOL_FILE=${_manifest}"
                    "-DNM_TOOL=${CMAKE_NM}" "-DALLOW_LEADING_UNDERSCORE=${_umicom_value_underscore}"
                    "-DREPORT=${CMAKE_CURRENT_BINARY_DIR}/value-link-tests/$<CONFIG>/${_owner}.json"
                    -P "${CMAKE_CURRENT_LIST_DIR}/VerifyLibrarySymbols.cmake")
            set_tests_properties("framework.value_link.archive.${_owner}" PROPERTIES
                TIMEOUT 90 LABELS "framework;build;link;production-archive")
        endif()
    endforeach()
endif()

if(BUILD_TESTING)
    add_subdirectory("${_umicom_value_root}/tests/implementation_fingerprint"
        "${CMAKE_CURRENT_BINARY_DIR}/implementation-fingerprint")
endif()
