# Umicom Framework
# File: cmake/UmicomProviderConnections.cmake
#
# PURPOSE:
# Compose shared provider settings, canonical identity and native presentation without duplicating persistence in applications.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

# A toolkit-independent owner lets products share durable settings and guarded
# credential acquisition without making storage or provider logic UI-specific.
# This module also supports independent composition; own its SDK registration dependency.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include_guard(GLOBAL)
include(GNUInstallDirs)
set(_umicom_connections_root "${CMAKE_CURRENT_LIST_DIR}/..")
add_library(umicom_provider_connections STATIC
    "${_umicom_connections_root}/src/provider_connections/validation.c"
    "${_umicom_connections_root}/src/provider_connections/wire.c"
    "${_umicom_connections_root}/src/provider_connections/store.c")
add_library(Umicom::provider_connections ALIAS umicom_provider_connections)
set_target_properties(umicom_provider_connections PROPERTIES EXPORT_NAME provider_connections
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_provider_connections PUBLIC
    $<BUILD_INTERFACE:${_umicom_connections_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_provider_connections PUBLIC Umicom::data Umicom::security)
umicom_apply_warnings(umicom_provider_connections)
umicom_apply_sanitizers(umicom_provider_connections)
install(TARGETS umicom_provider_connections EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
# Include this exported library surface in the opt-in SDK without changing full installs.
umicom_register_sdk_libraries(
    umicom_provider_connections
)
install(FILES "${_umicom_connections_root}/docs/learning/saving-provider-connections.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
if(BUILD_TESTING)
    add_executable(umicom-provider-connections-test "${_umicom_connections_root}/tests/provider_connections/test_connections.c")
    target_link_libraries(umicom-provider-connections-test PRIVATE Umicom::provider_connections)
    umicom_apply_warnings(umicom-provider-connections-test)
    umicom_apply_sanitizers(umicom-provider-connections-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-provider-connections-test)
    endif()
    foreach(case validation lifecycle stale scope limit acquire failed-secret transaction corrupt sqlite-reopen sqlite-failed-save sqlite-failed-delete boundaries orphan sqlite-lost-transaction sqlite-separate-server)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/provider-connections-tests/${case}")
        add_test(NAME framework.provider_connections.${case} COMMAND umicom-provider-connections-test ${case})
        set_tests_properties(framework.provider_connections.${case} PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/provider-connections-tests/${case}"
            LABELS "framework;provider-connections;privacy;persistence;regression")
    endforeach()
endif()
if(UMICOM_BUILD_LEARNING_EXAMPLES)
    add_executable(umicom-provider-connections-lesson "${_umicom_connections_root}/examples/provider_connections/saved_connections.c")
    target_link_libraries(umicom-provider-connections-lesson PRIVATE Umicom::provider_connections)
    umicom_apply_warnings(umicom-provider-connections-lesson)
    umicom_apply_sanitizers(umicom-provider-connections-lesson)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-provider-connections-lesson)
    endif()
endif()

# Native presentation belongs to the existing GTK owner so every product links
# one implementation. The persistence library remains toolkit-independent.
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${_umicom_connections_root}/adapters/gtk4/provider_connections_panel_gtk4.c"
        "${_umicom_connections_root}/adapters/gtk4/provider_connections_worker_gtk4.c"
        "${_umicom_connections_root}/adapters/gtk4/provider_connections_window_gtk4.c")
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::provider_connections)
endif()

if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-provider-connections-gtk-test "${_umicom_connections_root}/tests/provider_connections/test_editor_gtk4.c")
    target_link_libraries(umicom-provider-connections-gtk-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-provider-connections-gtk-test)
    umicom_apply_sanitizers(umicom-provider-connections-gtk-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-provider-connections-gtk-test)
    endif()
    foreach(case save-reopen conflict selection remove validation long-label close-guard failed-save bad-load retained pending-save-close invalid keys-entry)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/provider-editor-tests/${case}")
        add_test(NAME framework.provider_connections.gtk4.${case} COMMAND umicom-provider-connections-gtk-test ${case})
        set_tests_properties(framework.provider_connections.gtk4.${case} PROPERTIES TIMEOUT 40 SKIP_RETURN_CODE 77
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/provider-editor-tests/${case}"
            LABELS "framework;provider-connections;gtk4;persistence;ownership;regression")
    endforeach()
endif()

install(FILES "${_umicom_connections_root}/docs/learning/editing-provider-connections.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)

# Canonical identity uses the same wire owner and base SHA-256 implementation.
if(BUILD_TESTING)
    add_executable(umicom-provider-fingerprint-test "${_umicom_connections_root}/tests/provider_connections/test_fingerprint.c")
    target_link_libraries(umicom-provider-fingerprint-test PRIVATE Umicom::provider_connections)
    set_target_properties(umicom-provider-fingerprint-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-provider-fingerprint-test)
    umicom_apply_sanitizers(umicom-provider-fingerprint-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-provider-fingerprint-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-provider-fingerprint-test FILES
            "${_umicom_connections_root}/src/provider_connections/wire.c"
            "${_umicom_connections_root}/src/provider_connections/validation.c"
            "${_umicom_connections_root}/include/umicom/provider_connections/connections.h")
    endif()
    foreach(_case IN ITEMS oracle padding id provider label unicode endpoint model empty-model reference empty-reference timeout disabled loopback null-connection null-output unterminated bad-endpoint bad-reference bad-label bad-timeout bad-route store-roundtrip)
        set(_test "framework.provider_connections.fingerprint.${_case}")
        add_test(NAME "${_test}" COMMAND umicom-provider-fingerprint-test "${_case}")
        set_tests_properties("${_test}" PROPERTIES TIMEOUT 20 LABELS "framework;provider-connections;identity;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_test}" TARGETS umicom-provider-fingerprint-test)
        endif()
    endforeach()
endif()

# Provider receipts complement local workspace jobs; they do not run a scheduler.
target_sources(umicom_provider_connections PRIVATE
    "${_umicom_connections_root}/src/provider_connections/task_receipt.c")
if(BUILD_TESTING)
    add_executable(umicom-provider-task-receipt-test "${_umicom_connections_root}/tests/provider_connections/test_task_receipt.c")
    target_link_libraries(umicom-provider-task-receipt-test PRIVATE Umicom::provider_connections)
    set_target_properties(umicom-provider-task-receipt-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-provider-task-receipt-test)
    umicom_apply_sanitizers(umicom-provider-task-receipt-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-provider-task-receipt-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-provider-task-receipt-test FILES
            "${_umicom_connections_root}/src/provider_connections/task_receipt.c"
            "${_umicom_connections_root}/src/provider_connections/wire.c"
            "${_umicom_connections_root}/include/umicom/provider_connections/tasks.h")
    endif()
    foreach(_case IN ITEMS transition-matrix lifecycle prepare-refusals confirmation remote-id-bounds recovery-matrix recovery-binding output-refusals stale-and-overflow borrowed-inputs receipt-invariants null-and-unknown)
        set(_test "framework.provider_connections.task_receipt.${_case}")
        add_test(NAME "${_test}" COMMAND umicom-provider-task-receipt-test "${_case}")
        set_tests_properties("${_test}" PROPERTIES TIMEOUT 20 LABELS "framework;provider-connections;receipt;recovery;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_test}" TARGETS umicom-provider-task-receipt-test)
        endif()
    endforeach()
endif()
if(UMICOM_BUILD_LEARNING_EXAMPLES)
    add_executable(umicom-provider-task-lesson "${_umicom_connections_root}/examples/provider_connections/task_receipt.c")
    target_link_libraries(umicom-provider-task-lesson PRIVATE Umicom::provider_connections)
    set_target_properties(umicom-provider-task-lesson PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-provider-task-lesson)
    umicom_apply_sanitizers(umicom-provider-task-lesson)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationBranding.cmake")
    umicom_apply_application_branding(TARGET umicom-provider-task-lesson
        PRODUCT_NAME "Umicom Provider Task Lesson" INTERNAL_NAME "umicom-provider-task-lesson"
        APPLICATION_ID "org.umicom.provider-task-lesson" RESOURCE_ROOT "${_umicom_connections_root}/resources"
        NO_RUNTIME_DEPLOYMENT)
endif()

# Exact-byte receipt persistence is tested independently of provider/network effects.
if(BUILD_TESTING)
    add_executable(umicom-provider-task-wire-test "${_umicom_connections_root}/tests/provider_connections/test_task_wire.c")
    target_link_libraries(umicom-provider-task-wire-test PRIVATE Umicom::provider_connections)
    set_target_properties(umicom-provider-task-wire-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-provider-task-wire-test)
    umicom_apply_sanitizers(umicom-provider-task-wire-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-provider-task-wire-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-provider-task-wire-test FILES
            "${_umicom_connections_root}/src/provider_connections/wire.c"
            "${_umicom_connections_root}/src/provider_connections/task_receipt.c"
            "${_umicom_connections_root}/src/provider_connections/internal.h"
            "${_umicom_connections_root}/include/umicom/provider_connections/tasks.h")
    endif()
    foreach(_case IN ITEMS oracle phases wide-integers truncation schema numbers extent digests text invariants capacity padding invalid-input not-authentication)
        set(_test "framework.provider_connections.task_wire.${_case}")
        add_test(NAME "${_test}" COMMAND umicom-provider-task-wire-test "${_case}")
        set_tests_properties("${_test}" PROPERTIES TIMEOUT 20 LABELS "framework;provider-connections;receipt;serialization;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_test}" TARGETS umicom-provider-task-wire-test)
        endif()
    endforeach()
endif()

# Reuse the provider Data Server owner for durable remote task receipts.
target_sources(umicom_provider_connections PRIVATE
    "${_umicom_connections_root}/src/provider_connections/task_store.c")
if(BUILD_TESTING)
    add_executable(umicom-provider-task-store-test "${_umicom_connections_root}/tests/provider_connections/test_task_store.c")
    target_link_libraries(umicom-provider-task-store-test PRIVATE Umicom::provider_connections)
    set_target_properties(umicom-provider-task-store-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-provider-task-store-test)
    umicom_apply_sanitizers(umicom-provider-task-store-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-provider-task-store-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-provider-task-store-test FILES
            "${_umicom_connections_root}/src/provider_connections/task_store.c"
            "${_umicom_connections_root}/src/provider_connections/task_receipt.c"
            "${_umicom_connections_root}/src/provider_connections/store.c"
            "${_umicom_connections_root}/src/provider_connections/wire.c"
            "${_umicom_connections_root}/src/provider_connections/internal.h"
            "${_umicom_connections_root}/include/umicom/provider_connections/task_store.h"
            "${_umicom_connections_root}/include/umicom/provider_connections/tasks.h")
    endif()
    foreach(_case IN ITEMS lifecycle duplicate missing scope stale transaction corrupt identity borrowed-inputs invalid-input boundaries sqlite-reopen sqlite-failed-write sqlite-lost-transaction sqlite-separate-server sqlite-noop sqlite-readonly)
        set(_test "framework.provider_connections.task_store.${_case}")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/provider-task-store-tests/${_case}")
        add_test(NAME "${_test}" COMMAND umicom-provider-task-store-test "${_case}")
        set_tests_properties("${_test}" PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/provider-task-store-tests/${_case}"
            LABELS "framework;provider-connections;receipt;persistence;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_test}" TARGETS umicom-provider-task-store-test)
        endif()
    endforeach()
endif()

if(UMICOM_BUILD_LEARNING_EXAMPLES)
    add_executable(umicom-provider-stored-task-lesson "${_umicom_connections_root}/examples/provider_connections/stored_tasks.c")
    target_link_libraries(umicom-provider-stored-task-lesson PRIVATE Umicom::provider_connections)
    set_target_properties(umicom-provider-stored-task-lesson PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-provider-stored-task-lesson)
    umicom_apply_sanitizers(umicom-provider-stored-task-lesson)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationBranding.cmake")
    umicom_apply_application_branding(TARGET umicom-provider-stored-task-lesson
        PRODUCT_NAME "Umicom Stored Task Lesson" INTERNAL_NAME "umicom-provider-stored-task-lesson"
        APPLICATION_ID "org.umicom.stored-task-lesson" RESOURCE_ROOT "${_umicom_connections_root}/resources"
        NO_RUNTIME_DEPLOYMENT)
endif()

# Bounded discovery must not depend on backend visitation order or stale cursors.
if(BUILD_TESTING)
    add_executable(umicom-provider-task-catalogue-test "${_umicom_connections_root}/tests/provider_connections/test_task_catalogue.c")
    target_link_libraries(umicom-provider-task-catalogue-test PRIVATE Umicom::provider_connections)
    set_target_properties(umicom-provider-task-catalogue-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-provider-task-catalogue-test)
    umicom_apply_sanitizers(umicom-provider-task-catalogue-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-provider-task-catalogue-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-provider-task-catalogue-test FILES
            "${_umicom_connections_root}/src/provider_connections/task_store.c"
            "${_umicom_connections_root}/src/provider_connections/task_receipt.c"
            "${_umicom_connections_root}/src/provider_connections/wire.c"
            "${_umicom_connections_root}/src/provider_connections/store.c"
            "${_umicom_connections_root}/include/umicom/provider_connections/task_store.h")
    endif()
    foreach(_case IN ITEMS empty order filter stale filter-stale cursor scope corrupt schema malformed-key identity catalogue-limit invalid-query transaction insertion-order sqlite-pages sqlite-scan-limit sqlite-readonly)
        set(_test "framework.provider_connections.task_catalogue.${_case}")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/provider-task-catalogue-tests/${_case}")
        add_test(NAME "${_test}" COMMAND umicom-provider-task-catalogue-test "${_case}")
        set_tests_properties("${_test}" PROPERTIES TIMEOUT 60 SKIP_RETURN_CODE 77
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/provider-task-catalogue-tests/${_case}"
            LABELS "framework;provider-connections;receipt;catalogue;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_test}" TARGETS umicom-provider-task-catalogue-test)
        endif()
    endforeach()
endif()

# External consumers receive the same example sources without a Framework clone.
set(_umicom_provider_example_files
    "${_umicom_connections_root}/examples/provider_connections/CMakeLists.txt"
    "${_umicom_connections_root}/examples/provider_connections/saved_connections.c"
    "${_umicom_connections_root}/examples/provider_connections/task_receipt.c"
    "${_umicom_connections_root}/examples/provider_connections/stored_tasks.c")
install(FILES ${_umicom_provider_example_files}
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/provider_connections"
    COMPONENT Examples)
# The selective SDK uses FrameworkSDK throughout its artifact and metadata rules.
# Retain the unused component spelling for review; the active rule joins that same component.
if(FALSE)
install(FILES ${_umicom_provider_example_files}
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/provider_connections"
    COMPONENT SDK EXCLUDE_FROM_ALL)
endif()
install(FILES ${_umicom_provider_example_files}
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/provider_connections"
    COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)

# Retain one cooperative task effect owner using the existing platform lease.
# Persistence remains the canonical provider store; no new lock or database
# implementation is introduced by this composition.
target_sources(umicom_provider_connections PRIVATE
    "${_umicom_connections_root}/src/provider_connections/task_owner.c")
target_link_libraries(umicom_provider_connections PRIVATE Umicom::platform)
if(BUILD_TESTING)
    foreach(_suite IN ITEMS task-owner task-owner-failures)
        if(_suite STREQUAL "task-owner")
            set(_source test_task_owner.c)
            set(_cases path-stable path-scope path-boundaries lifecycle contention distinct-task distinct-scope stale-acquire changed-content stale-check wrong-scope missing-record missing-directory invalid existing-guard)
            set(_group task_owner)
        else()
            set(_source test_task_owner_failures.c)
            set(_cases allocation lease-busy read-failure changed cleanup-failure check-failure release-failure scope-mismatch invalid-path)
            set(_group task_owner_failures)
        endif()
        set(_target "umicom-provider-${_suite}-test")
        add_executable(${_target} "${_umicom_connections_root}/tests/provider_connections/${_source}")
        target_link_libraries(${_target} PRIVATE Umicom::provider_connections Umicom::platform)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${_umicom_connections_root}/src/provider_connections/task_owner.c"
                "${_umicom_connections_root}/include/umicom/provider_connections/task_owner.h"
                "${_umicom_connections_root}/src/provider_connections/task_store.c"
                "${_umicom_connections_root}/src/provider_connections/task_receipt.c"
                "${_umicom_connections_root}/src/platform/file_lease.c")
        endif()
        foreach(_case IN LISTS _cases)
            set(_test "framework.provider_connections.${_group}.${_case}")
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/provider-task-owner-tests/${_group}/${_case}")
            add_test(NAME "${_test}" COMMAND ${_target} "${_case}")
            set_tests_properties("${_test}" PROPERTIES TIMEOUT 30
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/provider-task-owner-tests/${_group}/${_case}"
                LABELS "framework;provider-connections;ownership;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners("${_test}" TARGETS ${_target})
            endif()
        endforeach()
    endforeach()
endif()
