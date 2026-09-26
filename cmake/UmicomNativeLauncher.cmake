#-----------------------------------------------------------------------------
# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Native C23 launcher-delivery capability. No Python, shell or code-generator
# process is required to build or execute this target and its primary tests.
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)
set(_umi_native_root "${CMAKE_CURRENT_LIST_DIR}/..")
add_library(umicom_native_launcher STATIC
    "${_umi_native_root}/src/native_launcher/sha256.c"
    "${_umi_native_root}/src/native_launcher/text.c"
    "${_umi_native_root}/src/native_launcher/entry.c"
    "${_umi_native_root}/src/native_launcher/formats.c"
    "${_umi_native_root}/src/native_launcher/manifest.c"
    "${_umi_native_root}/src/native_launcher/plan.c"
    "${_umi_native_root}/src/native_launcher/brand.c"
    "${_umi_native_root}/src/native_launcher/cli.c")
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(umicom_native_launcher PRIVATE "${_umi_native_root}/src/native_launcher/io_linux.c")
else()
    target_sources(umicom_native_launcher PRIVATE "${_umi_native_root}/src/native_launcher/io_unsupported.c")
    if(WIN32)
        target_sources(umicom_native_launcher PRIVATE "${_umi_native_root}/src/native_launcher/digest_windows.c")
    endif()
endif()
add_library(Umicom::native_launcher ALIAS umicom_native_launcher)
set_target_properties(umicom_native_launcher PROPERTIES EXPORT_NAME native_launcher
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_native_launcher PUBLIC
    $<BUILD_INTERFACE:${_umi_native_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_native_launcher)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_native_launcher)
endif()
option(UMICOM_NATIVE_LAUNCHER_BUILD_TOOLS "Build native C launcher staging and file-digest tool" ON)
if(UMICOM_NATIVE_LAUNCHER_BUILD_TOOLS)
    add_executable(umicom-session-stage "${_umi_native_root}/examples/native_launcher/main.c")
    target_link_libraries(umicom-session-stage PRIVATE Umicom::native_launcher)
    set_target_properties(umicom-session-stage PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(MINGW)
        target_link_options(umicom-session-stage PRIVATE -municode)
    endif()
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-session-stage)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-session-stage)
    endif()
    install(TARGETS umicom-session-stage RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
endif()
if(BUILD_TESTING)
    add_subdirectory("${_umi_native_root}/tests/native_launcher" "${CMAKE_CURRENT_BINARY_DIR}/native-launcher-tests")
endif()
install(TARGETS umicom_native_launcher EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(DIRECTORY "${_umi_native_root}/include/umicom/native_launcher"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom COMPONENT Framework)
install(FILES "${_umi_native_root}/docs/learning/native-launcher-delivery.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
unset(_umi_native_root)
