# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# This projection consumes existing semantic document types; no alternate schema.
include_guard(GLOBAL)
include(GNUInstallDirs)
get_filename_component(_umi_viewport_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_workbench_viewport STATIC "${_umi_viewport_root}/src/workbench_viewport/viewport.c")
add_library(Umicom::workbench_viewport ALIAS umicom_workbench_viewport)
set_target_properties(umicom_workbench_viewport PROPERTIES EXPORT_NAME workbench_viewport)
target_include_directories(umicom_workbench_viewport PUBLIC
    $<BUILD_INTERFACE:${_umi_viewport_root}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
add_executable(umicom-layout-example
    "${_umi_viewport_root}/examples/workbench_viewport/main.c"
    "${_umi_viewport_root}/examples/workbench_viewport/practice_layout.c")
target_link_libraries(umicom-layout-example PRIVATE Umicom::workbench_viewport)
foreach(_target IN ITEMS umicom_workbench_viewport umicom-layout-example)
    set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${_target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${_target})
    endif()
endforeach()
install(TARGETS umicom_workbench_viewport EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(TARGETS umicom-layout-example RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Learning)
install(FILES "${_umi_viewport_root}/include/umicom/workbench_layout/viewport.h"
    "${_umi_viewport_root}/include/umicom/workbench_layout/viewport_gtk4.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/workbench_layout COMPONENT Framework)
install(FILES "${_umi_viewport_root}/docs/learning/arrange-a-workspace.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
if(BUILD_TESTING)
    add_subdirectory("${_umi_viewport_root}/tests/workbench_viewport" "${CMAKE_CURRENT_BINARY_DIR}/workbench-viewport-tests")
    add_subdirectory("${_umi_viewport_root}/tests/cmake/sqlite_target" "${CMAKE_CURRENT_BINARY_DIR}/sqlite-target-tests")
endif()
unset(_umi_viewport_root)
