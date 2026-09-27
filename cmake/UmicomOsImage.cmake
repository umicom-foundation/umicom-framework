#-----------------------------------------------------------------------------
# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Native image work reuses Framework process ownership, SHA-256 and boot reports.
# The OS repository supplies build profiles, guest PID 1 and recovery policy.
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)
find_package(Threads REQUIRED)
if(NOT TARGET Umicom::native_launcher)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomNativeLauncher.cmake")
endif()
if(NOT TARGET Umicom::platform)
    message(FATAL_ERROR "Native OS image tools require canonical Umicom::platform")
endif()
set(_umi_os_image_root "${CMAKE_CURRENT_LIST_DIR}/..")
add_library(umicom_os_image STATIC
    "${_umi_os_image_root}/src/os_image/text.c"
    "${_umi_os_image_root}/src/os_image/archive.c"
    "${_umi_os_image_root}/src/os_image/formats.c"
    "${_umi_os_image_root}/src/os_image/profile.c"
    "${_umi_os_image_root}/src/os_image/prepare.c"
    "${_umi_os_image_root}/src/os_image/process.c"
    "${_umi_os_image_root}/src/os_image/build.c"
    "${_umi_os_image_root}/src/os_image/bundle.c"
    "${_umi_os_image_root}/src/os_image/boot.c"
    "${_umi_os_image_root}/src/os_image/cli.c")
if(WIN32)
    target_sources(umicom_os_image PRIVATE "${_umi_os_image_root}/src/os_image/io_windows.c")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(umicom_os_image PRIVATE "${_umi_os_image_root}/src/os_image/io_linux.c")
else()
    target_sources(umicom_os_image PRIVATE "${_umi_os_image_root}/src/os_image/io_unsupported.c")
endif()
add_library(Umicom::os_image ALIAS umicom_os_image)
set_target_properties(umicom_os_image PROPERTIES EXPORT_NAME os_image
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_os_image PUBLIC
    $<BUILD_INTERFACE:${_umi_os_image_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_os_image PUBLIC Umicom::native_launcher Umicom::platform PRIVATE Threads::Threads)
add_executable(umicom-os-image "${_umi_os_image_root}/examples/os_image/main.c")
target_link_libraries(umicom-os-image PRIVATE Umicom::os_image Threads::Threads)
set_target_properties(umicom-os-image PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
add_executable(umicom-os-image-archive-example "${_umi_os_image_root}/examples/os_image/archive_lesson.c")
target_link_libraries(umicom-os-image-archive-example PRIVATE Umicom::os_image)
set_target_properties(umicom-os-image-archive-example PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MINGW)
    target_link_options(umicom-os-image PRIVATE -municode)
endif()
foreach(_target IN ITEMS umicom_os_image umicom-os-image umicom-os-image-archive-example)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${_target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${_target})
    endif()
endforeach()
install(TARGETS umicom_os_image EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(TARGETS umicom-os-image RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Framework)
install(DIRECTORY "${_umi_os_image_root}/include/umicom/os_image"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom COMPONENT Framework)
install(FILES "${_umi_os_image_root}/docs/learning/build-and-check-umicom-os.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
if(BUILD_TESTING)
    add_subdirectory("${_umi_os_image_root}/tests/os_image" "${CMAKE_CURRENT_BINARY_DIR}/os-image-tests")
endif()
unset(_umi_os_image_root)
