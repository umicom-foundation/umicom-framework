# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# File storage is an optional native adapter; the planning library remains pure.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomKanbanStorageSources.cmake")
include(GNUInstallDirs)
if(NOT TARGET Umicom::kanban OR NOT TARGET Umicom::platform)
    message(FATAL_ERROR "Kanban storage requires Umicom::kanban and Umicom::platform")
endif()
include_guard(GLOBAL)
add_library(umicom_kanban_storage STATIC)
add_library(Umicom::kanban_storage ALIAS umicom_kanban_storage)
umicom_add_kanban_storage_sources(umicom_kanban_storage)
install(TARGETS umicom_kanban_storage EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
umicom_register_sdk_libraries(umicom_kanban_storage)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_kanban_storage include/umicom/kanban/document.h include/umicom/kanban/location.h)
endif()
if(BUILD_TESTING)
    include("${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_storage/tests.cmake")
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/kanban/location.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/kanban" COMPONENT Framework)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/project-planning-boards.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides" COMPONENT Framework)
