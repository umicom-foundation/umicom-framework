# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Planning state is reusable native domain logic with no GTK or network dependency.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomKanbanSources.cmake")
include(GNUInstallDirs)
if(NOT TARGET Umicom::base)
    message(FATAL_ERROR "Kanban composition requires Umicom::base")
endif()
include_guard(GLOBAL)
get_filename_component(_kanban_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_kanban STATIC)
add_library(Umicom::kanban ALIAS umicom_kanban)
umicom_add_kanban_sources(umicom_kanban)
install(TARGETS umicom_kanban EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
umicom_register_sdk_libraries(umicom_kanban)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_kanban
        include/umicom/kanban/board.h include/umicom/kanban/view.h include/umicom/kanban/archive.h)
endif()
# Root installation owns include/ for both full and selective SDKs.
install(FILES "${_kanban_root}/docs/KANBAN_PLANNING.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs" COMPONENT Framework)
install(FILES "${_kanban_root}/docs/KANBAN_PLANNING.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs"
    COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)
if(BUILD_TESTING)
    include("${_kanban_root}/tests/kanban/tests.cmake")
endif()

# Hosts without native platform services can still compose the pure board library.
if(TARGET Umicom::platform)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomKanbanStorage.cmake")
endif()
