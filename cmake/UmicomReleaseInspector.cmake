#-----------------------------------------------------------------------------
# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# The release inspector is a native consumer of Setup Centre's canonical
# inventory and checked I/O. Build descriptions do not parse PE files.
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)
if(NOT TARGET Umicom::setup_centre)
    message(FATAL_ERROR "The release inspector requires Umicom::setup_centre")
endif()
set(_umi_inspect_root "${CMAKE_CURRENT_LIST_DIR}/..")
add_library(umicom_release_inspector STATIC
    "${_umi_inspect_root}/src/release_inspector/pe.c"
    "${_umi_inspect_root}/src/release_inspector/inspection.c"
    "${_umi_inspect_root}/src/release_inspector/cli.c")
add_library(Umicom::release_inspector ALIAS umicom_release_inspector)
set_target_properties(umicom_release_inspector PROPERTIES EXPORT_NAME release_inspector
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_release_inspector PUBLIC Umicom::setup_centre)
target_include_directories(umicom_release_inspector PUBLIC
    $<BUILD_INTERFACE:${_umi_inspect_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
add_executable(umicom-release-inspect "${_umi_inspect_root}/examples/release_inspector/main.c")
target_link_libraries(umicom-release-inspect PRIVATE Umicom::release_inspector)
set_target_properties(umicom-release-inspect PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MINGW)
    target_link_options(umicom-release-inspect PRIVATE -municode -static-libgcc)
endif()
foreach(_target IN ITEMS umicom_release_inspector umicom-release-inspect)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${_target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${_target})
    endif()
endforeach()
if(TARGET umicom-setup-centre)
    target_link_libraries(umicom-setup-centre PRIVATE Umicom::release_inspector)
endif()
install(TARGETS umicom_release_inspector EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(TARGETS umicom-release-inspect RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
install(DIRECTORY "${_umi_inspect_root}/include/umicom/release_inspector"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom COMPONENT Framework)
install(FILES "${_umi_inspect_root}/docs/learning/check-a-windows-release.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)

option(UMICOM_RELEASE_NOTES_EXAMPLE "Build the Windows Notes release laboratory" ON)
if(WIN32 AND UMICOM_RELEASE_NOTES_EXAMPLE AND NOT TARGET umicom-release-notes)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../examples/release_notes"
        "${CMAKE_CURRENT_BINARY_DIR}/release-notes-example")
endif()

if(BUILD_TESTING)
    add_subdirectory("${_umi_inspect_root}/tests/release_inspector"
        "${CMAKE_CURRENT_BINARY_DIR}/release-inspector-tests")
endif()
unset(_umi_inspect_root)
