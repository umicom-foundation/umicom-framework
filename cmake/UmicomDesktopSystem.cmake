#-----------------------------------------------------------------------------
# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Shared host observations and explicit Linux session handoff.
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)
if(NOT TARGET Umicom::platform)
    message(FATAL_ERROR "Desktop System requires the canonical Umicom::platform target")
endif()
set(_umi_desktop_system_root "${CMAKE_CURRENT_LIST_DIR}/..")
add_library(umicom_desktop_system STATIC
    "${_umi_desktop_system_root}/src/desktop_system/parse.c"
    "${_umi_desktop_system_root}/src/desktop_system/monitor.c"
    "${_umi_desktop_system_root}/src/desktop_system/report.c"
    "${_umi_desktop_system_root}/src/desktop_system/session.c")
add_library(Umicom::desktop_system ALIAS umicom_desktop_system)
set_target_properties(umicom_desktop_system PROPERTIES EXPORT_NAME desktop_system
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(WIN32)
    target_sources(umicom_desktop_system PRIVATE
        "${_umi_desktop_system_root}/src/desktop_system/windows.c"
        "${_umi_desktop_system_root}/src/desktop_system/session_unsupported.c")
    target_link_libraries(umicom_desktop_system PRIVATE iphlpapi ws2_32)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(umicom_desktop_system PRIVATE
        "${_umi_desktop_system_root}/src/desktop_system/linux.c"
        "${_umi_desktop_system_root}/src/desktop_system/session_linux.c")
else()
    target_sources(umicom_desktop_system PRIVATE
        "${_umi_desktop_system_root}/src/desktop_system/unsupported.c"
        "${_umi_desktop_system_root}/src/desktop_system/session_unsupported.c")
endif()
target_include_directories(umicom_desktop_system PUBLIC
    $<BUILD_INTERFACE:${_umi_desktop_system_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_desktop_system PUBLIC Umicom::platform)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_desktop_system)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_desktop_system)
endif()
install(TARGETS umicom_desktop_system EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(DIRECTORY "${_umi_desktop_system_root}/include/umicom/desktop_system"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom COMPONENT Framework)
option(UMICOM_DESKTOP_SYSTEM_BUILD_TOOLS "Build read-only system report and Linux session launcher" ON)
if(UMICOM_DESKTOP_SYSTEM_BUILD_TOOLS)
    add_executable(umicom-system-report "${_umi_desktop_system_root}/examples/desktop_system/report_main.c")
    add_executable(umicom-desk-session "${_umi_desktop_system_root}/examples/desktop_system/session_main.c")
    foreach(_target IN ITEMS umicom-system-report umicom-desk-session)
        target_link_libraries(${_target} PRIVATE Umicom::desktop_system)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(${_target})
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(${_target})
        endif()
        install(TARGETS ${_target} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
    endforeach()
endif()
if(BUILD_TESTING)
    add_subdirectory("${_umi_desktop_system_root}/tests/desktop_system"
        "${CMAKE_CURRENT_BINARY_DIR}/desktop-system-tests")
endif()
install(FILES "${_umi_desktop_system_root}/docs/learning/desk-system-centre.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
unset(_umi_desktop_system_root)

# Native C owns launcher staging and verification. The earlier Python script
# remains unchanged as an optional comparison and legacy-workflow alternative.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomNativeLauncher.cmake")
