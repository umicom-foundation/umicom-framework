#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/windows_deployment/test_integration.cmake
#
# PURPOSE:
#   Configure the actual branding/deployment helpers in nested target scopes.
#   This is a graph/configuration test, not a Windows linker or execution test.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
set(_source "${WORK}/source with spaces")
set(_build "${WORK}/build with spaces")
file(MAKE_DIRECTORY "${_source}/resources/brand" "${_source}/resources/windows"
    "${_source}/runtime/bin" "${_source}/second")
foreach(_name IN ITEMS umicom-icon.svg umicom-icon-on-dark.svg umicom-logo.svg
        umicom-logo-on-dark.svg umicom.ico)
    file(WRITE "${_source}/resources/brand/${_name}" "configuration-only fixture\n")
endforeach()
file(WRITE "${_source}/resources/windows/umicom-application.rc.in" "/* configuration-only */\n")
file(WRITE "${_source}/runtime/bin/libglib-2.0-0.dll" "configuration-only fixture\n")
file(WRITE "${_source}/main.c" "int main(void) { return 0; }\n")
set(HELPER "${FRAMEWORK}/cmake")
if(MODE STREQUAL "installed")
    set(HELPER "${WORK}/sdk/lib/cmake/UmicomFramework")
    file(MAKE_DIRECTORY "${HELPER}")
    file(COPY "${FRAMEWORK}/cmake/UmicomApplicationBranding.cmake"
        "${FRAMEWORK}/cmake/UmicomWindowsDeployment.cmake" DESTINATION "${HELPER}")
    file(COPY "${FRAMEWORK}/cmake/windows" DESTINATION "${HELPER}")
    # Installed resources and native support are siblings under the SDK's share tree.
    file(COPY "${_source}/resources" DESTINATION "${WORK}/sdk/share/umicom/framework")
    foreach(_file IN ITEMS src/delivery/windows_runtime_bootstrap.c
            src/delivery/windows_runtime_cache.c include/umicom/delivery/windows_runtime_cache.h)
        get_filename_component(_parent "${_file}" DIRECTORY)
        file(MAKE_DIRECTORY "${WORK}/sdk/share/umicom/framework/deployment/${_parent}")
        file(COPY "${FRAMEWORK}/${_file}"
            DESTINATION "${WORK}/sdk/share/umicom/framework/deployment/${_parent}")
    endforeach()
    set(RESOURCE_ROOT "${WORK}/sdk/share/umicom/framework/resources")
else()
    set(RESOURCE_ROOT "${_source}/resources")
endif()
set(_project [=[
cmake_minimum_required(VERSION 3.24)
project(UmicomDeploymentGraph VERSION 0.3.0 LANGUAGES C)
# Simulate platform metadata for configuration only; no executable is built.
set(WIN32 TRUE)
set(CMAKE_HOST_WIN32 TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(UMICOM_WINDOWS_RUNTIME_PREFIX "${CMAKE_SOURCE_DIR}/runtime" CACHE PATH "" FORCE)
set(CMAKE_OBJDUMP "${CMAKE_COMMAND}")
include("@HELPER@/UmicomApplicationBranding.cmake")
set(RESOURCE_ROOT "@RESOURCE_ROOT@")
add_executable(demo main.c)
umicom_apply_application_branding(TARGET demo PRODUCT_NAME "Umicom Demo"
    INTERNAL_NAME demo RESOURCE_ROOT "${RESOURCE_ROOT}" WINDOWS_GUI)
add_executable(console main.c)
umicom_apply_application_branding(TARGET console PRODUCT_NAME "Umicom Console"
    INTERNAL_NAME console RESOURCE_ROOT "${RESOURCE_ROOT}")
add_subdirectory(second)
]=])
string(CONFIGURE "${_project}" _project @ONLY)
file(WRITE "${_source}/CMakeLists.txt" "${_project}")
file(WRITE "${_source}/second/CMakeLists.txt" [=[
add_executable(umicom ../main.c)
add_executable(other ../main.c)
umicom_apply_application_branding(TARGET other PRODUCT_NAME "Umicom Other"
    INTERNAL_NAME other RESOURCE_ROOT "${RESOURCE_ROOT}" WINDOWS_GUI)
]=])
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${_source}" -B "${_build}"
    -G Ninja -DCMAKE_BUILD_TYPE=Debug
    RESULT_VARIABLE _result OUTPUT_VARIABLE _stdout ERROR_VARIABLE _stderr)
if(NOT _result STREQUAL "0")
    message(FATAL_ERROR "Nested deployment graph failed to configure:\n${_stdout}\n${_stderr}")
endif()
file(READ "${_build}/umicom-deployment/Debug/suite.cmake" _catalogue)
if(NOT _catalogue MATCHES "demo;other" OR _catalogue MATCHES "CONFIG_console"
        OR NOT _catalogue MATCHES "UMI_PACKAGE_HELPERS.*umicom")
    message(FATAL_ERROR "Installer catalogue did not select only GUI targets:\n${_catalogue}")
endif()
file(READ "${_build}/build.ninja" _graph)
if(NOT _graph MATCHES "umicom-windows-installer" OR NOT _graph MATCHES "demo-windows-runtime")
    message(FATAL_ERROR "Deployment build targets are missing")
endif()
file(READ "${_build}/cmake_install.cmake" _install)
string(FIND "${_install}" [=[${CMAKE_INSTALL_PREFIX}]=] _prefix_position)
if(_prefix_position LESS 0 OR NOT _install MATCHES "InstallRuntime.cmake")
    message(FATAL_ERROR "Install-time prefix/configuration was not preserved")
endif()
# Also exercise the fail-closed subset check in the deferred finaliser.
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${_source}" -B "${WORK}/invalid subset"
    -G Ninja -DUMICOM_WINDOWS_INSTALLER_TARGETS=not-built
    RESULT_VARIABLE _result OUTPUT_VARIABLE _stdout ERROR_VARIABLE _stderr)
if(_result STREQUAL "0" OR NOT _stderr MATCHES "not a registered, enabled GUI application")
    message(FATAL_ERROR "An unavailable application was accepted into the installer")
endif()
