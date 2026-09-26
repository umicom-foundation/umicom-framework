#-----------------------------------------------------------------------------
# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# C23-only installer core; native Windows GUI avoids requiring GTK to install GTK.
# Existing runtime staging and the NSIS installer are retained unchanged.
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)
set(_umi_setup_root "${CMAKE_CURRENT_LIST_DIR}/..")
if(NOT TARGET Umicom::native_launcher)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomNativeLauncher.cmake")
endif()
add_library(umicom_setup_centre STATIC
    "${_umi_setup_root}/src/setup_centre/text.c"
    "${_umi_setup_root}/src/setup_centre/catalogue.c"
    "${_umi_setup_root}/src/setup_centre/io.c"
    "${_umi_setup_root}/src/setup_centre/package.c"
    "${_umi_setup_root}/src/setup_centre/install.c"
    "${_umi_setup_root}/src/setup_centre/virtual_machine.c"
    "${_umi_setup_root}/src/setup_centre/media.c"
    "${_umi_setup_root}/src/setup_centre/shortcuts.c"
    "${_umi_setup_root}/src/setup_centre/cli.c")
add_library(Umicom::setup_centre ALIAS umicom_setup_centre)
set_target_properties(umicom_setup_centre PROPERTIES EXPORT_NAME setup_centre
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_setup_centre PUBLIC
    $<BUILD_INTERFACE:${_umi_setup_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_setup_centre PUBLIC Umicom::native_launcher)
if(WIN32)
    target_link_libraries(umicom_setup_centre PRIVATE ole32 shell32 uuid)
endif()
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_setup_centre)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_setup_centre)
endif()
if(TARGET Umicom::platform)
    add_library(umicom_setup_process STATIC "${_umi_setup_root}/src/setup_centre/process_adapter.c")
    add_library(Umicom::setup_process ALIAS umicom_setup_process)
    set_target_properties(umicom_setup_process PROPERTIES EXPORT_NAME setup_process
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom_setup_process PUBLIC Umicom::setup_centre Umicom::platform)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom_setup_process)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom_setup_process)
    endif()
    install(TARGETS umicom_setup_process EXPORT UmicomFrameworkTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
endif()
add_executable(umicom-setup "${_umi_setup_root}/examples/setup_centre/main.c")
target_link_libraries(umicom-setup PRIVATE Umicom::setup_centre)
set_target_properties(umicom-setup PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(TARGET Umicom::setup_process)
    target_link_libraries(umicom-setup PRIVATE Umicom::setup_process)
    target_compile_definitions(umicom-setup PRIVATE UMICOM_SETUP_HAS_PROCESS=1)
endif()
if(MINGW)
    target_link_options(umicom-setup PRIVATE -municode -static-libgcc)
endif()
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-setup)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-setup)
endif()
if(WIN32 AND TARGET Umicom::setup_process)
    enable_language(RC)
    add_executable(umicom-setup-centre WIN32
        "${_umi_setup_root}/examples/setup_centre/windows_main.c"
        "${_umi_setup_root}/adapters/win32/setup_centre.c"
        "${_umi_setup_root}/examples/setup_centre/resources/setup.rc")
    target_link_libraries(umicom-setup-centre PRIVATE Umicom::setup_process
        comctl32 comdlg32 user32 gdi32 ole32 shell32 uuid)
    target_include_directories(umicom-setup-centre PRIVATE "${_umi_setup_root}/examples/setup_centre/resources")
    target_compile_definitions(umicom-setup-centre PRIVATE UNICODE _UNICODE)
    set_target_properties(umicom-setup-centre PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(MINGW)
        target_link_options(umicom-setup-centre PRIVATE -municode -static-libgcc)
    endif()
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-setup-centre)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-setup-centre)
    endif()
    install(TARGETS umicom-setup-centre RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
endif()
install(TARGETS umicom_setup_centre EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(TARGETS umicom-setup RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
install(DIRECTORY "${_umi_setup_root}/include/umicom/setup_centre"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom COMPONENT Framework)
install(FILES "${_umi_setup_root}/docs/learning/install-and-try-umicom.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
if(BUILD_TESTING)
    add_subdirectory("${_umi_setup_root}/tests/setup_centre" "${CMAKE_CURRENT_BINARY_DIR}/setup-centre-tests")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSetupPackage.cmake")
if(WIN32 AND TARGET umicom-setup-centre AND NOT CMAKE_CROSSCOMPILING)
    cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" CALL umicom_setup_package_targets)
endif()
unset(_umi_setup_root)
