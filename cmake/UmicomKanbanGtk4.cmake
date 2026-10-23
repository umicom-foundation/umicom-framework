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
    "${_kanban_gtk_root}/adapters/gtk4/kanban_window.c")
add_library(Umicom::kanban_gtk4 ALIAS umicom_kanban_gtk4)
set_target_properties(umicom_kanban_gtk4 PROPERTIES
    EXPORT_NAME kanban_gtk4 C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_kanban_gtk4 PUBLIC Umicom::kanban_storage Umicom::ui_gtk4)
umicom_apply_warnings(umicom_kanban_gtk4)
umicom_apply_sanitizers(umicom_kanban_gtk4)
install(TARGETS umicom_kanban_gtk4 EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
umicom_register_sdk_libraries(umicom_kanban_gtk4)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_kanban_gtk4
        include/umicom/ui/gtk4/kanban.h include/umicom/ui/gtk4/kanban_window.h)
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
