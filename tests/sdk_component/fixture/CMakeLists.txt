# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Build small real libraries so SDK component tests exercise CMake's generated
# install/export behavior without compiling the entire Framework again.
cmake_minimum_required(VERSION 3.24)
project(UmicomSdkComponentFixture LANGUAGES C)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include("${SDK_HELPER}")

if(TEST_CASE STREQUAL "shared")
    set(_kind SHARED)
else()
    set(_kind STATIC)
endif()
add_library(sdk_base ${_kind} base.c)
set_target_properties(sdk_base PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
target_include_directories(sdk_base PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
add_library(sdk_value STATIC value.c)
target_link_libraries(sdk_value PUBLIC sdk_base)
add_library(sdk_interface INTERFACE)
target_link_libraries(sdk_interface INTERFACE sdk_value)
install(TARGETS sdk_base sdk_value sdk_interface EXPORT SdkFixtureTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

# These refusal cases must fail with the helper's diagnostic, not a later link
# failure. They cover mistakes an optional platform module could introduce.
if(TEST_CASE STREQUAL "missing-target")
    umicom_register_sdk_libraries(does_not_exist)
elseif(TEST_CASE STREQUAL "imported-target")
    add_library(sdk_foreign STATIC IMPORTED)
    umicom_register_sdk_libraries(sdk_foreign)
elseif(TEST_CASE STREQUAL "alias-target")
    add_library(Sdk::alias ALIAS sdk_base)
    umicom_register_sdk_libraries(Sdk::alias)
elseif(TEST_CASE STREQUAL "executable-target")
    add_executable(sdk_wrong tool.c)
    umicom_register_sdk_libraries(sdk_wrong)
elseif(TEST_CASE STREQUAL "object-target")
    add_library(sdk_objects OBJECT base.c)
    umicom_register_sdk_libraries(sdk_objects)
elseif(TEST_CASE STREQUAL "empty-registration")
    umicom_register_sdk_libraries()
elseif(TEST_CASE STREQUAL "name-collision")
    add_custom_target(umicom-sdk-libraries)
endif()

umicom_register_sdk_libraries(sdk_base sdk_value sdk_interface)
if(TEST_CASE STREQUAL "duplicate-registration")
    umicom_register_sdk_libraries(sdk_value sdk_base)
endif()
if(TEST_CASE STREQUAL "deferred-registration")
    # A later callback must extend the already-created aggregate build target.
    cmake_language(DEFER CALL umicom_register_sdk_libraries sdk_deferred)
    add_library(sdk_deferred STATIC late.c)
    target_link_libraries(sdk_interface INTERFACE sdk_deferred)
    install(TARGETS sdk_deferred EXPORT SdkFixtureTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
endif()

# The SDK build must not depend on this normal product executable. A component
# install after building libraries only must succeed with its artifact absent.
add_executable(sdk_tool tool.c)
install(TARGETS sdk_tool RUNTIME DESTINATION bin)
install(FILES sdk_fixture.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(FILES sdk_fixture.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)
configure_package_config_file(SdkFixtureConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/SdkFixtureConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SdkFixture")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/SdkFixtureConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SdkFixture")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/SdkFixtureConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SdkFixture"
    COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)
install(EXPORT SdkFixtureTargets NAMESPACE Sdk:: FILE SdkFixtureTargets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SdkFixture")
install(EXPORT SdkFixtureTargets NAMESPACE Sdk:: FILE SdkFixtureTargets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SdkFixture"
    COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/tool-path-$<CONFIG>.txt"
    CONTENT "$<TARGET_FILE:sdk_tool>")

# Exported developer commands must accompany the libraries without dragging
# unrelated product applications into the selective build.
if(TEST_CASE MATCHES "^(development-command|duplicate-tool|relocated-tool|missing-installed-tool)$")
    add_executable(sdk_development_command tool.c)
    install(TARGETS sdk_development_command EXPORT SdkFixtureTargets
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
    umicom_register_sdk_tools(sdk_development_command)
    if(TEST_CASE STREQUAL "duplicate-tool")
        umicom_register_sdk_tools(sdk_development_command)
    endif()
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/development-tool-$<CONFIG>.txt"
        CONTENT "$<TARGET_FILE:sdk_development_command>")
elseif(TEST_CASE STREQUAL "tool-missing")
    umicom_register_sdk_tools(does_not_exist)
elseif(TEST_CASE STREQUAL "tool-library")
    umicom_register_sdk_tools(sdk_base)
elseif(TEST_CASE STREQUAL "tool-imported")
    add_executable(sdk_imported_command IMPORTED)
    umicom_register_sdk_tools(sdk_imported_command)
elseif(TEST_CASE STREQUAL "tool-alias")
    add_executable(Sdk::tool_alias ALIAS sdk_tool)
    umicom_register_sdk_tools(Sdk::tool_alias)
elseif(TEST_CASE STREQUAL "tool-empty")
    umicom_register_sdk_tools()
elseif(TEST_CASE STREQUAL "tool-bundle")
    set_target_properties(sdk_tool PROPERTIES MACOSX_BUNDLE TRUE)
    umicom_register_sdk_tools(sdk_tool)
endif()

# Run after the optional deferred library registration above.
cmake_language(DEFER CALL umicom_generate_sdk_artifact_inventory)
