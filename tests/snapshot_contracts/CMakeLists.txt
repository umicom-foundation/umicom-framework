#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/snapshot_contracts/CMakeLists.txt
# PURPOSE: Link each typed consumer against its real public owning library.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
get_property(_contracts GLOBAL PROPERTY UMICOM_SNAPSHOT_CONTRACT_ENTRIES)
if(NOT _contracts)
    message(FATAL_ERROR "Snapshot contract tests require their explicit ownership list")
endif()
get_property(_scoped_contracts GLOBAL PROPERTY UMICOM_SNAPSHOT_DOCUMENT_CONTRACTS)
add_custom_target(umicom-snapshot-contract-checks)
foreach(_contract IN LISTS _contracts)
    string(REPLACE "/" ";" _parts "${_contract}")
    list(GET _parts 0 _group)
    list(GET _parts 1 _name)
    set(_target "umicom-snapshot-${_group}-${_name}-test")
    add_executable("${_target}" "test_${_group}_${_name}.c")
    target_link_libraries("${_target}" PRIVATE "Umicom::${_group}")
    set_target_properties("${_target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES
        C_EXTENSIONS NO UNITY_BUILD OFF DISABLE_PRECOMPILE_HEADERS YES)
    umicom_apply_warnings("${_target}")
    umicom_apply_sanitizers("${_target}")
    add_dependencies(umicom-snapshot-contract-checks "${_target}")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("${_target}")
    endif()
    if(_contract IN_LIST _scoped_contracts)
        foreach(_case scope scope-empty scope-reject scope-capacity scope-stale scope-alias)
            set(_test "framework.snapshot_contracts.${_group}.${_name}.${_case}")
            add_test(NAME "${_test}" COMMAND "${_target}" "${_case}")
            set_tests_properties("${_test}" PROPERTIES TIMEOUT 120
                LABELS "framework;snapshot-contracts;document-publication;${_group};regression")
        endforeach()
    endif()
    foreach(_case lifecycle fields boundaries invalid batch batch-invalid batch-duplicate batch-capacity batch-empty)
        set(_test "framework.snapshot_contracts.${_group}.${_name}.${_case}")
        add_test(NAME "${_test}" COMMAND "${_target}" "${_case}")
        set_tests_properties("${_test}" PROPERTIES TIMEOUT 120
            LABELS "framework;snapshot-contracts;${_group};regression")
    endforeach()
endforeach()

add_executable(umicom-snapshot-validation-test test_validation.c)
target_link_libraries(umicom-snapshot-validation-test PRIVATE Umicom::base)
umicom_apply_warnings(umicom-snapshot-validation-test)
umicom_apply_sanitizers(umicom-snapshot-validation-test)
add_dependencies(umicom-snapshot-contract-checks umicom-snapshot-validation-test)
foreach(_case valid null schema bounds immutable messages)
    add_test(NAME "framework.snapshot_contracts.validation.${_case}"
        COMMAND umicom-snapshot-validation-test "${_case}")
    set_tests_properties("framework.snapshot_contracts.validation.${_case}" PROPERTIES
        TIMEOUT 30 LABELS "framework;snapshot-contracts;base;regression")
endforeach()

# The canonical lesson is both an in-tree consumer and an installed-SDK example.
add_executable(umicom-snapshot-batch-example ../../examples/snapshot_contracts/main.c)
target_link_libraries(umicom-snapshot-batch-example PRIVATE Umicom::platform)
umicom_apply_warnings(umicom-snapshot-batch-example)
umicom_apply_sanitizers(umicom-snapshot-batch-example)
add_dependencies(umicom-snapshot-contract-checks umicom-snapshot-batch-example)
add_test(NAME framework.snapshot_contracts.example COMMAND umicom-snapshot-batch-example)
set_tests_properties(framework.snapshot_contracts.example PROPERTIES TIMEOUT 30
    LABELS "framework;snapshot-contracts;example")
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-snapshot-validation-test)
    umicom_register_validation_target(umicom-snapshot-batch-example)
endif()

# One production instantiation exercises the shared staging allocation path.
# Linker wrapping cannot intercept allocations inside a separate shared library.
get_target_property(_platform_type umicom_platform TYPE)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang"
        AND _platform_type STREQUAL "STATIC_LIBRARY")
    add_executable(umicom-snapshot-allocation-test test_platform_bookmarks.c)
    target_link_libraries(umicom-snapshot-allocation-test PRIVATE Umicom::platform)
    target_compile_definitions(umicom-snapshot-allocation-test PRIVATE UMI_TEST_SNAPSHOT_ALLOCATION=1)
    target_link_options(umicom-snapshot-allocation-test PRIVATE -Wl,--wrap=malloc)
    umicom_apply_warnings(umicom-snapshot-allocation-test)
    umicom_apply_sanitizers(umicom-snapshot-allocation-test)
    add_dependencies(umicom-snapshot-contract-checks umicom-snapshot-allocation-test)
    add_test(NAME framework.snapshot_contracts.allocation COMMAND umicom-snapshot-allocation-test allocation)
    set_tests_properties(framework.snapshot_contracts.allocation PROPERTIES TIMEOUT 30
        LABELS "framework;snapshot-contracts;fault-injection")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-snapshot-allocation-test)
    endif()
endif()

add_executable(umicom-document-replace-example ../../examples/snapshot_contracts/document_replace.c)
target_link_libraries(umicom-document-replace-example PRIVATE Umicom::language)
umicom_apply_warnings(umicom-document-replace-example)
umicom_apply_sanitizers(umicom-document-replace-example)
add_dependencies(umicom-snapshot-contract-checks umicom-document-replace-example)
add_test(NAME framework.snapshot_contracts.document-example COMMAND umicom-document-replace-example)
set_tests_properties(framework.snapshot_contracts.document-example PROPERTIES
    TIMEOUT 30 LABELS "framework;snapshot-contracts;document-publication;example")
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-document-replace-example)
endif()

# One real scoped registry covers failed staging for replacement and clearing.
get_target_property(_language_type umicom_language TYPE)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang"
        AND _language_type STREQUAL "STATIC_LIBRARY")
    add_executable(umicom-document-allocation-test test_language_diagnostic.c)
    target_link_libraries(umicom-document-allocation-test PRIVATE Umicom::language)
    target_compile_definitions(umicom-document-allocation-test PRIVATE UMI_TEST_SNAPSHOT_ALLOCATION=1)
    target_link_options(umicom-document-allocation-test PRIVATE -Wl,--wrap=malloc)
    umicom_apply_warnings(umicom-document-allocation-test)
    umicom_apply_sanitizers(umicom-document-allocation-test)
    add_dependencies(umicom-snapshot-contract-checks umicom-document-allocation-test)
    add_test(NAME framework.snapshot_contracts.document-allocation
        COMMAND umicom-document-allocation-test scope-allocation)
    set_tests_properties(framework.snapshot_contracts.document-allocation PROPERTIES
        TIMEOUT 30 LABELS "framework;snapshot-contracts;document-publication;fault-injection")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-allocation-test)
    endif()
endif()
