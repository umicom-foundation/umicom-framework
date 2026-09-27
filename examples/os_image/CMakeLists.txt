# Umicom Foundation | Sammy Hegab | MIT
# Focused consumer composition using canonical source implementations. It is
# deliberately not represented as the complete Framework or Applications build.
cmake_minimum_required(VERSION 3.25)
project(UmicomOsImageTools LANGUAGES C)
include(CTest)
include(GNUInstallDirs)
find_package(Threads REQUIRED)
get_filename_component(UMICOM_FRAMEWORK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
option(UMICOM_OS_IMAGE_SANITIZE "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    if(UMICOM_OS_IMAGE_SANITIZE)
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()
add_library(umicom_image_platform STATIC
    "${UMICOM_FRAMEWORK_ROOT}/src/platform/process.c"
    "${UMICOM_FRAMEWORK_ROOT}/src/platform/cancellation.c"
    "${UMICOM_FRAMEWORK_ROOT}/src/platform/boot_report.c")
add_library(Umicom::platform ALIAS umicom_image_platform)
set_target_properties(umicom_image_platform PROPERTIES EXPORT_NAME platform)
target_include_directories(umicom_image_platform PUBLIC
    $<BUILD_INTERFACE:${UMICOM_FRAMEWORK_ROOT}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_image_platform PUBLIC Threads::Threads)
install(TARGETS umicom_image_platform EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES "${UMICOM_FRAMEWORK_ROOT}/include/umicom/base/status.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/base)
install(FILES "${UMICOM_FRAMEWORK_ROOT}/include/umicom/platform/process.h"
    "${UMICOM_FRAMEWORK_ROOT}/include/umicom/platform/cancellation.h"
    "${UMICOM_FRAMEWORK_ROOT}/include/umicom/platform/boot_report.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/platform)
include("${UMICOM_FRAMEWORK_ROOT}/cmake/UmicomOsImage.cmake")
install(EXPORT UmicomFrameworkTargets NAMESPACE Umicom::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UmicomImageSDK)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/UmicomImageSDKConfig.cmake"
    "include(CMakeFindDependencyMacro)\nfind_dependency(Threads)\ninclude(\"\${CMAKE_CURRENT_LIST_DIR}/UmicomFrameworkTargets.cmake\")\n")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/UmicomImageSDKConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UmicomImageSDK)

# The retained launcher tests only read this inert binary as a byte fixture.
# Do not inflate it with an ASan runtime: its descriptor-lifetime test repeatedly
# hashes it under an already-instrumented library. The new executable pipeline
# fixture and every native image/library/test implementation remain instrumented.
if(UMICOM_OS_IMAGE_SANITIZE AND TARGET umicom-native-launcher-inert-child
        AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-native-launcher-inert-child PRIVATE -fno-sanitize=all)
    target_link_options(umicom-native-launcher-inert-child PRIVATE -fno-sanitize=all)
endif()
