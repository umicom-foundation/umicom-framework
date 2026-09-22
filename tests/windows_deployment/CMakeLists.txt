#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/windows_deployment/CMakeLists.txt
#
# PURPOSE:
#   Provide a small independently buildable regression project for Windows
#   deployment contracts. No full application suite or GTK build is required.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomWindowsDeploymentTests LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
option(UMICOM_WINDOWS_DEPLOYMENT_TEST_SANITIZERS "Enable focused ASan/UBSan checks" OFF)
get_filename_component(UMI_FRAMEWORK "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
add_executable(umicom-windows-deployment-test
    test_runtime_cache.c "${UMI_FRAMEWORK}/src/delivery/windows_runtime_cache.c")
target_include_directories(umicom-windows-deployment-test PRIVATE "${UMI_FRAMEWORK}/include")
if(MSVC)
    target_compile_options(umicom-windows-deployment-test PRIVATE /W4 /WX)
else()
    target_compile_options(umicom-windows-deployment-test PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    if(UMICOM_WINDOWS_DEPLOYMENT_TEST_SANITIZERS)
        target_compile_options(umicom-windows-deployment-test PRIVATE
            -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(umicom-windows-deployment-test PRIVATE -fsanitize=address,undefined)
    endif()
endif()
foreach(_case IN ITEMS empty unchanged single repeated partial unicode backslashes
        short query exact invalid-root nul null-arguments dollar many)
    add_test(NAME windows_deployment.cache.${_case}
        COMMAND umicom-windows-deployment-test "${_case}")
endforeach()
set(_success_cases safe-values escape copy same-copy pe-x64 pe-x86 owned-reset render
    render-dollar render-unicode deterministic nonwindows-noop)
set(_failure_cases bad-identifier bad-literal bad-bracket bad-newline parent-path
    absolute-path drive-path backslash-path reserved-path trailing-dot tab-path
    duplicate-path missing-file directory-file invalid-pe truncated-pe wrong-architecture
    unowned-reset render-empty render-missing render-duplicate-path render-duplicate-title
    render-invalid-version render-control-file render-missing-exe render-bad-title)
if(NOT WIN32)
    list(APPEND _failure_cases symlink-file)
endif()
foreach(_case IN LISTS _success_cases _failure_cases)
    set(_driver "${CMAKE_CURRENT_LIST_DIR}/test_deployment.cmake")
    if(_case IN_LIST _failure_cases)
        set(_driver "${CMAKE_CURRENT_LIST_DIR}/expect_refusal.cmake")
    endif()
    add_test(NAME windows_deployment.cmake.${_case}
        COMMAND "${CMAKE_COMMAND}"
            "-DCASE=${_case}"
            "-DFRAMEWORK=${UMI_FRAMEWORK}"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/cases/${_case}"
            "-DFIXTURE_EXE=$<TARGET_FILE:umicom-windows-deployment-test>"
            -P "${_driver}")
endforeach()
add_test(NAME windows_deployment.cmake.target-integration
    COMMAND "${CMAKE_COMMAND}"
        "-DFRAMEWORK=${UMI_FRAMEWORK}"
        "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/integration"
        -P "${CMAKE_CURRENT_LIST_DIR}/test_integration.cmake")

add_test(NAME windows_deployment.cmake.installed-sdk-integration
    COMMAND "${CMAKE_COMMAND}" -DMODE=installed
        "-DFRAMEWORK=${UMI_FRAMEWORK}"
        "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/sdk-integration"
        -P "${CMAKE_CURRENT_LIST_DIR}/test_integration.cmake")

# This is a real Win32 startup test, not a Linux mock. It is registered only on
# a Windows host and is reported separately from portable contract coverage.
if(WIN32)
    add_executable(umicom-windows-startup-test
        test_windows_startup.c
        "${UMI_FRAMEWORK}/src/delivery/windows_runtime_cache.c"
        "${UMI_FRAMEWORK}/src/delivery/windows_runtime_bootstrap.c")
    target_include_directories(umicom-windows-startup-test PRIVATE "${UMI_FRAMEWORK}/include")
    target_link_libraries(umicom-windows-startup-test PRIVATE user32)
    set_target_properties(umicom-windows-startup-test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/startup/bin"
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    if(MINGW)
        target_link_options(umicom-windows-startup-test PRIVATE -static -static-libgcc)
    endif()
    add_custom_command(TARGET umicom-windows-startup-test POST_BUILD
        COMMAND "${CMAKE_COMMAND}" "-DROOT=$<TARGET_FILE_DIR:umicom-windows-startup-test>"
            -P "${CMAKE_CURRENT_LIST_DIR}/prepare_startup_test.cmake" VERBATIM)
    add_test(NAME windows_deployment.native.startup-environment
        COMMAND umicom-windows-startup-test)
    set_tests_properties(windows_deployment.native.startup-environment PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif()

# Exercise the real staging script in a child cmake -P process. The probe stops
# before PE scanning or deployment, so it is safe on Windows and non-Windows
# hosts. On CMake 4.3+ it checks NEW policy selection; older hosts check that the
# unknown-policy guard remains compatible with the supported minimum version.
foreach(_mode IN ITEMS default inherited-old)
    add_test(NAME windows_deployment.cmake.runtime-policy-${_mode}
        COMMAND "${CMAKE_COMMAND}"
            "-DFRAMEWORK=${UMI_FRAMEWORK}"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/runtime-policy/${_mode}"
            "-DMODE=${_mode}"
            -P "${CMAKE_CURRENT_LIST_DIR}/test_runtime_policy.cmake")
endforeach()
