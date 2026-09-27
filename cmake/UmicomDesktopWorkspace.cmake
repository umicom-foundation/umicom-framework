# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Shared persisted drafts and checkpoints. The GUI remains a separate adapter.
include_guard(GLOBAL)
if(TARGET Umicom::desktop_workspace)
    return()
endif()
include(GNUInstallDirs)
foreach(_dependency IN ITEMS Umicom::data Umicom::native_launcher)
    if(NOT TARGET ${_dependency})
        message(FATAL_ERROR "Desktop workspace requires ${_dependency}")
    endif()
endforeach()
set(_dw_root "${CMAKE_CURRENT_LIST_DIR}/..")
add_library(umicom_desktop_workspace STATIC
    "${_dw_root}/src/desktop_workspace/model.c"
    "${_dw_root}/src/desktop_workspace/codec.c"
    "${_dw_root}/src/desktop_workspace/repository.c"
    "${_dw_root}/src/desktop_workspace/local.c"
    "${_dw_root}/src/desktop_workspace/cli.c")
add_library(Umicom::desktop_workspace ALIAS umicom_desktop_workspace)
set_target_properties(umicom_desktop_workspace PROPERTIES EXPORT_NAME desktop_workspace
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_desktop_workspace PUBLIC
    $<BUILD_INTERFACE:${_dw_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_desktop_workspace PUBLIC Umicom::data Umicom::native_launcher)
add_executable(umicom-desktop-workspace "${_dw_root}/examples/desktop_workspace/main.c")
if(MINGW)
    target_link_options(umicom-desktop-workspace PRIVATE -municode)
endif()
add_executable(umicom-desktop-workspace-example "${_dw_root}/examples/desktop_workspace/notes_example.c")
foreach(_target IN ITEMS umicom-desktop-workspace umicom-desktop-workspace-example)
    target_link_libraries(${_target} PRIVATE Umicom::desktop_workspace)
    set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
endforeach()
foreach(_target IN ITEMS umicom_desktop_workspace umicom-desktop-workspace umicom-desktop-workspace-example)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${_target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${_target})
    endif()
endforeach()
install(TARGETS umicom_desktop_workspace EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(TARGETS umicom-desktop-workspace umicom-desktop-workspace-example
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
install(DIRECTORY "${_dw_root}/include/umicom/desktop_workspace"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom COMPONENT Framework)
install(FILES "${_dw_root}/docs/learning/desktop-workspace.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
if(BUILD_TESTING)
    add_subdirectory("${_dw_root}/tests/desktop_workspace" "${CMAKE_CURRENT_BINARY_DIR}/desktop-workspace-tests")
endif()
unset(_dw_root)
