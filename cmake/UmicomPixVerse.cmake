# Umicom Framework
# File: cmake/UmicomPixVerse.cmake
#
# PURPOSE:
# Compose native video transport, shared task recovery and their offline regression coverage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

# Video generation shares local credentials and the configured curl dependency.
# This module also supports independent composition; own its SDK registration dependency.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include_guard(GLOBAL)
include(GNUInstallDirs)
set(_umicom_pixverse_root "${CMAKE_CURRENT_LIST_DIR}/..")
add_library(umicom_pixverse STATIC
    "${_umicom_pixverse_root}/src/media_generation/pixverse_plan.c"
    "${_umicom_pixverse_root}/src/media_generation/pixverse_request.c"
    "${_umicom_pixverse_root}/src/media_generation/pixverse_response.c"
    "${_umicom_pixverse_root}/src/media_generation/pixverse_http.c")
add_library(Umicom::pixverse ALIAS umicom_pixverse)
set_target_properties(umicom_pixverse PROPERTIES EXPORT_NAME pixverse C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_pixverse PUBLIC $<BUILD_INTERFACE:${_umicom_pixverse_root}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_pixverse PUBLIC Umicom::security Umicom::platform PRIVATE Umicom::developer Umicom::ai)
if(UMICOM_PROVIDER_CONNECTION_CHECK_HTTP_BUILT)
    target_compile_definitions(umicom_pixverse PRIVATE UMICOM_PIXVERSE_HAS_HTTP=1)
    target_link_libraries(umicom_pixverse PRIVATE PkgConfig::UMICOM_CONNECTION_CHECK_CURL)
endif()
umicom_apply_warnings(umicom_pixverse)
umicom_apply_sanitizers(umicom_pixverse)
target_link_libraries(umicom_framework INTERFACE Umicom::pixverse)
install(TARGETS umicom_pixverse EXPORT UmicomFrameworkTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
# Include this exported library surface in the opt-in SDK without changing full installs.
umicom_register_sdk_libraries(
    umicom_pixverse
)
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE "${_umicom_pixverse_root}/adapters/gtk4/pixverse_panel_gtk4.c")
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::pixverse)
endif()

if(BUILD_TESTING)
    add_executable(umicom-pixverse-test "${CMAKE_CURRENT_LIST_DIR}/../tests/media_generation/test_pixverse.c")
    target_link_libraries(umicom-pixverse-test PRIVATE Umicom::pixverse)
    umicom_apply_warnings(umicom-pixverse-test)
    umicom_apply_sanitizers(umicom-pixverse-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-pixverse-test)
    endif()
    foreach(case create settings immutable status-deleted status-moderated status-failed bad-trace bad-ratio bad-quality bad-duration empty utf8 unterminated denied key-injection cancel-before cancel-receipt timeout redirect rate-limit overflow duplicate provider-error wrong-type trailing status status-pending status-once status-id status-fields status-mismatch status-unknown status-url)
        add_test(NAME framework.pixverse.${case} COMMAND umicom-pixverse-test ${case})
        set_tests_properties(framework.pixverse.${case} PROPERTIES TIMEOUT 30 LABELS "framework;creative;regression")
    endforeach()
endif()
if(BUILD_TESTING AND UMICOM_PROVIDER_CONNECTION_CHECK_HTTP_BUILT)
    add_executable(umicom-pixverse-http-test "${CMAKE_CURRENT_LIST_DIR}/../tests/media_generation/test_pixverse_http.c")
    target_link_libraries(umicom-pixverse-http-test PRIVATE Umicom::pixverse PkgConfig::UMICOM_CONNECTION_CHECK_CURL)
    umicom_apply_warnings(umicom-pixverse-http-test)
    umicom_apply_sanitizers(umicom-pixverse-http-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-pixverse-http-test)
    endif()
    foreach(case get post redirect denied rate-limit timeout cancel overflow option-failure)
        add_test(NAME framework.pixverse.http.${case} COMMAND umicom-pixverse-http-test ${case})
        set_tests_properties(framework.pixverse.http.${case} PROPERTIES TIMEOUT 30 LABELS "framework;creative;regression")
    endforeach()
    target_compile_definitions(umicom-pixverse-http-test PRIVATE UMICOM_PIXVERSE_HAS_HTTP=1)
endif()
if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-pixverse-gtk-test "${CMAKE_CURRENT_LIST_DIR}/../tests/media_generation/test_pixverse_gtk4.c")
    target_link_libraries(umicom-pixverse-gtk-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-pixverse-gtk-test)
    umicom_apply_sanitizers(umicom-pixverse-gtk-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-pixverse-gtk-test)
    endif()
    foreach(case review status status-invalid unapproved alias password prompt ratio seconds quality audio shots retained)
        add_test(NAME framework.pixverse.gtk.${case} COMMAND umicom-pixverse-gtk-test ${case})
        set_tests_properties(framework.pixverse.gtk.${case} PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative;regression;gtk4")
    endforeach()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/pixverse-video-requests.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Learning)

# The provider owner supplies transport; shared task services own persistence.
target_sources(umicom_pixverse PRIVATE "${_umicom_pixverse_root}/src/media_generation/pixverse_tasks.c")
target_link_libraries(umicom_pixverse PUBLIC Umicom::provider_connections)

if(BUILD_TESTING)
    add_executable(umicom-pixverse-task-test "${_umicom_pixverse_root}/tests/media_generation/test_pixverse_tasks.c")
    target_link_libraries(umicom-pixverse-task-test PRIVATE Umicom::pixverse)
    set_target_properties(umicom-pixverse-task-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-pixverse-task-test)
    umicom_apply_sanitizers(umicom-pixverse-task-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-pixverse-task-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-pixverse-task-test FILES
            "${_umicom_pixverse_root}/src/media_generation/pixverse_tasks.c"
            "${_umicom_pixverse_root}/src/media_generation/pixverse_plan.c"
            "${_umicom_pixverse_root}/src/media_generation/pixverse_request.c"
            "${_umicom_pixverse_root}/src/media_generation/pixverse_response.c"
            "${_umicom_pixverse_root}/src/provider_connections/task_store.c"
            "${_umicom_pixverse_root}/src/provider_connections/task_receipt.c"
            "${_umicom_pixverse_root}/include/umicom/media_generation/pixverse_tasks.h")
    endif()
    foreach(_case IN ITEMS prepare request-identity connection scope unconsumed submission uncertain-timeout cancel-before cancel-receipt creation-binding creation-flags stale duplicate store-reopen store-transaction status-running status-ready status-deleted status-moderated status-failed status-mismatch status-unknown status-cancel status-consumed recovery-binding recovery-id recovery-terminal revision-limit invalid-input)
        set(_test "framework.pixverse.tasks.${_case}")
        add_test(NAME "${_test}" COMMAND umicom-pixverse-task-test "${_case}")
        set_tests_properties("${_test}" PROPERTIES TIMEOUT 30 LABELS "framework;creative;provider-connections;receipt;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_test}" TARGETS umicom-pixverse-task-test)
        endif()
    endforeach()
endif()

if(UMICOM_BUILD_LEARNING_EXAMPLES)
    add_executable(umicom-pixverse-task-lesson "${_umicom_pixverse_root}/examples/media_generation/pixverse_task_recovery.c")
    target_link_libraries(umicom-pixverse-task-lesson PRIVATE Umicom::pixverse)
    set_target_properties(umicom-pixverse-task-lesson PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-pixverse-task-lesson)
    umicom_apply_sanitizers(umicom-pixverse-task-lesson)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationBranding.cmake")
    umicom_apply_application_branding(TARGET umicom-pixverse-task-lesson
        PRODUCT_NAME "Umicom Video Task Lesson" INTERNAL_NAME "umicom-pixverse-task-lesson"
        APPLICATION_ID "org.umicom.video-task-lesson" RESOURCE_ROOT "${_umicom_pixverse_root}/resources"
        NO_RUNTIME_DEPLOYMENT)
endif()

# Keep the example beside the canonical installed documentation so its
# relative source link works in full, selective SDK and documentation installs.
set(_umicom_pixverse_example_files
    "${_umicom_pixverse_root}/examples/media_generation/CMakeLists.txt"
    "${_umicom_pixverse_root}/examples/media_generation/pixverse_task_recovery.c")
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES ${_umicom_pixverse_example_files}
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/media_generation"
        COMPONENT "${_component}" ${_selection})
endforeach()
