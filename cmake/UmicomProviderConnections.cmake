# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# A toolkit-independent owner lets products share durable settings and guarded
# credential acquisition without making storage or provider logic UI-specific.
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
