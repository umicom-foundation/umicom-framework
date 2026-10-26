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
        include/umicom/kanban/board.h include/umicom/kanban/view.h include/umicom/kanban/archive.h
        include/umicom/kanban/publication.h include/umicom/kanban/developers.h)
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

# Offline native workflow documentation accompanies both full and SDK installs.
install(FILES "${_kanban_root}/docs/guides/native-kanban-and-contributor-view.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides" COMPONENT Framework)
install(FILES "${_kanban_root}/docs/guides/native-kanban-and-contributor-view.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides"
    COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)

# Reviewed public fields have a separate private archive from board documents.
install(FILES "${_kanban_root}/include/umicom/kanban/publication_plan.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/kanban" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_kanban include/umicom/kanban/publication_plan.h)
endif()
install(FILES "${_kanban_root}/docs/guides/saved-publication-plans.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides" COMPONENT Framework)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomKanbanPublicationPlan.cmake")

# Completion review is optional and retains private evidence outside the board archive.
install(FILES "${_kanban_root}/include/umicom/kanban/completion_plan.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/kanban" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_kanban include/umicom/kanban/completion_plan.h)
endif()
include("${CMAKE_CURRENT_LIST_DIR}/UmicomKanbanCompletion.cmake")

foreach(component Framework FrameworkSDK)
    install(FILES "${_kanban_root}/docs/guides/kanban-completion-review.html"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides" COMPONENT ${component})
endforeach()
unset(component)

# Private detail edits preserve all fields absent from a host form.
install(FILES "${_kanban_root}/include/umicom/kanban/card_details.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/kanban" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_kanban include/umicom/kanban/card_details.h)
endif()
include("${CMAKE_CURRENT_LIST_DIR}/UmicomKanbanCardDetails.cmake")
