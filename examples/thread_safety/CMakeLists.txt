#-----------------------------------------------------------------------------
# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# A focused SDK built from the canonical platform source, not a substitute runtime.
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomThreadSafety VERSION 0.1.0 LANGUAGES C)
include(CTest)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
find_package(Threads REQUIRED)
set(UMICOM_FRAMEWORK_SOURCE "${CMAKE_CURRENT_LIST_DIR}/../.." CACHE PATH "Framework source checkout")
get_filename_component(UMICOM_FRAMEWORK_SOURCE "${UMICOM_FRAMEWORK_SOURCE}" ABSOLUTE)
if(NOT EXISTS "${UMICOM_FRAMEWORK_SOURCE}/src/platform/threading.c")
    message(FATAL_ERROR "UMICOM_FRAMEWORK_SOURCE must contain the canonical platform threading source")
endif()
option(UMICOM_THREAD_SAFETY_STRICT "Treat focused-source warnings as errors" ON)
option(UMICOM_THREAD_SAFETY_SANITIZERS "Instrument focused native code with ASan and UBSan" OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
add_library(umicom_platform STATIC "${UMICOM_FRAMEWORK_SOURCE}/src/platform/threading.c")
add_library(Umicom::platform ALIAS umicom_platform)
set_target_properties(umicom_platform PROPERTIES EXPORT_NAME platform)
target_include_directories(umicom_platform PUBLIC
    $<BUILD_INTERFACE:${UMICOM_FRAMEWORK_SOURCE}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_platform PUBLIC Threads::Threads)
include("${UMICOM_FRAMEWORK_SOURCE}/cmake/UmicomThreadSafety.cmake")
umicom_thread_safety_target(umicom_platform)
install(TARGETS umicom_platform EXPORT UmicomThreadSafetyTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES "${UMICOM_FRAMEWORK_SOURCE}/include/umicom/platform/threading.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/platform)
install(FILES "${UMICOM_FRAMEWORK_SOURCE}/include/umicom/base/status.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/base)
install(EXPORT UmicomThreadSafetyTargets NAMESPACE Umicom:: DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/UmicomThreadSafety)
configure_package_config_file("${CMAKE_CURRENT_LIST_DIR}/UmicomThreadSafetyConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/UmicomThreadSafetyConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UmicomThreadSafety)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/UmicomThreadSafetyConfigVersion.cmake"
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/UmicomThreadSafetyConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/UmicomThreadSafetyConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UmicomThreadSafety)
