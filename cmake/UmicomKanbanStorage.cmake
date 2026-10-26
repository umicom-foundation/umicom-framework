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

install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/kanban/project_binding.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/kanban/binding_storage.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/kanban" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_kanban_storage
        include/umicom/kanban/project_binding.h include/umicom/kanban/binding_storage.h)
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/remembered-project-boards.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides" COMPONENT Framework)

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/kanban/publication_plan_storage.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/kanban" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_kanban_storage include/umicom/kanban/publication_plan_storage.h)
endif()

# Private completion evidence is retained separately from mutable board files.
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/kanban/completion_storage.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/kanban" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_kanban_storage include/umicom/kanban/completion_storage.h)
endif()

# Public examples use installed exports; private review archives are never installed.
foreach(component Framework FrameworkSDK)
    install(FILES
        "${CMAKE_CURRENT_LIST_DIR}/../examples/kanban_completion/CMakeLists.txt"
        "${CMAKE_CURRENT_LIST_DIR}/../examples/kanban_completion/main.c"
        "${CMAKE_CURRENT_LIST_DIR}/../examples/kanban_completion/check.cmake"
        "${CMAKE_CURRENT_LIST_DIR}/../examples/kanban_completion/README.md"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/kanban_completion"
        COMPONENT ${component})
endforeach()
unset(component)
