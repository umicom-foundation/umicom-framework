# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Headless planning stays available without GTK. Guard only after prerequisites
# exist so a later include can compose the adapter after its native providers.
if(NOT TARGET Umicom::ui_gtk4 OR NOT TARGET Umicom::kanban_storage)
    return()
endif()
include_guard(GLOBAL)
include(GNUInstallDirs)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomTextExport.cmake")
get_filename_component(_kanban_gtk_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_kanban_gtk4 STATIC
    "${_kanban_gtk_root}/adapters/gtk4/kanban.c"
    "${_kanban_gtk_root}/adapters/gtk4/kanban_publication_store.c"
    "${_kanban_gtk_root}/adapters/gtk4/kanban_completion.c"
    "${_kanban_gtk_root}/adapters/gtk4/kanban_dock.c"
    "${_kanban_gtk_root}/adapters/gtk4/kanban_window.c")
add_library(Umicom::kanban_gtk4 ALIAS umicom_kanban_gtk4)
set_target_properties(umicom_kanban_gtk4 PROPERTIES
    EXPORT_NAME kanban_gtk4 C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_kanban_gtk4 PUBLIC Umicom::kanban_storage Umicom::ui_gtk4)

# The optional evidence picker is enabled by the actual native provider, not
# merely by a testing library name. Manual private review remains independent.
get_target_property(_kanban_report_provider umicom_ui_gtk4 UMICOM_CTEST_REPORT_BROWSER_AVAILABLE)
if(_kanban_report_provider)
    target_compile_definitions(umicom_kanban_gtk4 PRIVATE UMICOM_KANBAN_SAVED_REPORT=1)
endif()
unset(_kanban_report_provider)

umicom_apply_warnings(umicom_kanban_gtk4)
umicom_apply_sanitizers(umicom_kanban_gtk4)
install(TARGETS umicom_kanban_gtk4 EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
umicom_register_sdk_libraries(umicom_kanban_gtk4)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_kanban_gtk4
        include/umicom/ui/gtk4/kanban.h include/umicom/ui/gtk4/kanban_window.h include/umicom/ui/gtk4/kanban_project.h)
endif()
if(UMICOM_BUILD_NATIVE_TOOL)
    add_executable(umicom-kanban "${_kanban_gtk_root}/tools/kanban/main.c")
    target_link_libraries(umicom-kanban PRIVATE Umicom::kanban_gtk4)
    set_target_properties(umicom-kanban PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-kanban)
    umicom_apply_sanitizers(umicom-kanban)
    if(COMMAND umicom_apply_application_branding)
        umicom_apply_application_branding(TARGET umicom-kanban PRODUCT_NAME "Umicom Kanban"
            INTERNAL_NAME "umicom-kanban" APPLICATION_ID "org.umicom.Kanban")
    endif()
    install(TARGETS umicom-kanban RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT Framework)
endif()
if(BUILD_TESTING)
    include("${_kanban_gtk_root}/tests/kanban_gtk4/tests.cmake")
endif()

install(FILES "${_kanban_gtk_root}/include/umicom/ui/gtk4/kanban_project.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)

install(FILES "${_kanban_gtk_root}/include/umicom/ui/gtk4/kanban_binding.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_kanban_gtk4 include/umicom/ui/gtk4/kanban_binding.h)
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_kanban_gtk4 FILES
        "${_kanban_gtk_root}/adapters/gtk4/kanban_project.inc"
        "${_kanban_gtk_root}/adapters/gtk4/kanban_publication_plan.inc"
        "${_kanban_gtk_root}/adapters/gtk4/kanban_completion_bridge.inc"
        "${_kanban_gtk_root}/adapters/gtk4/kanban_card_details.inc"
        "${_kanban_gtk_root}/adapters/gtk4/kanban_card_labels.inc"
        "${_kanban_gtk_root}/adapters/gtk4/kanban_card_deadline.inc"
        "${_kanban_gtk_root}/adapters/gtk4/kanban_card_archive.inc"
        "${_kanban_gtk_root}/adapters/gtk4/kanban_view_filter.inc"
        "${_kanban_gtk_root}/adapters/gtk4/kanban_column_policy.inc"
        "${_kanban_gtk_root}/adapters/gtk4/kanban_card_checklist.inc"
        "${_kanban_gtk_root}/adapters/gtk4/kanban_card_dependencies.inc"
        "${_kanban_gtk_root}/adapters/gtk4/kanban_completion_report.inc"
        "${_kanban_gtk_root}/adapters/gtk4/kanban_binding.inc"
        "${_kanban_gtk_root}/adapters/gtk4/focus_handoff_private.h")
endif()

install(FILES "${_kanban_gtk_root}/include/umicom/ui/gtk4/kanban_publication_store.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_kanban_gtk4 include/umicom/ui/gtk4/kanban_publication_store.h)
endif()
include("${CMAKE_CURRENT_LIST_DIR}/UmicomKanbanPublicationPlan.cmake")

install(FILES "${_kanban_gtk_root}/include/umicom/ui/gtk4/kanban_dock.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_kanban_gtk4 include/umicom/ui/gtk4/kanban_dock.h)
endif()

# Native tests may be composed after the headless Studio catalogue.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomKanbanDock.cmake")

install(FILES "${_kanban_gtk_root}/docs/guides/embedded-kanban-board.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs" COMPONENT Framework)

if(BUILD_TESTING)
    add_executable(umi-kanban-public-freshness-test "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_gtk4/test_public_freshness.c")
    target_link_libraries(umi-kanban-public-freshness-test PRIVATE Umicom::kanban_gtk4)
    set_target_properties(umi-kanban-public-freshness-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-kanban-public-freshness-test)
    umicom_apply_sanitizers(umi-kanban-public-freshness-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-kanban-public-freshness-test)
    endif()
    foreach(case normal include-change title-change board-edit external external-refresh generate-refused clear retired reentrant-title reentrant-edit reentrant-export regenerate refused-edit accepted-snapshot)
        set(test "framework.kanban.publication_freshness.${case}")
        set(directory "${CMAKE_CURRENT_BINARY_DIR}/qualification/framework.kanban.publication_freshness/${case}")
        file(MAKE_DIRECTORY "${directory}")
        add_test(NAME ${test} COMMAND umi-kanban-public-freshness-test ${case} "${directory}")
        set_tests_properties(${test} PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77
            WORKING_DIRECTORY "${directory}" ENVIRONMENT "G_DEBUG=fatal-warnings"
            LABELS "framework;gtk4;publication;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-kanban-public-freshness-test)
        endif()
    endforeach()
endif()

# Completion review is shared by standalone and embedded native board hosts.
install(FILES "${_kanban_gtk_root}/include/umicom/ui/gtk4/kanban_completion.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_kanban_gtk4 include/umicom/ui/gtk4/kanban_completion.h)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/UmicomKanbanCompletionNative.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomKanbanCardDetailsNative.cmake")
