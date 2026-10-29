#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/learning_pack_inspector/CMakeLists.txt
#
# PURPOSE:
#   Build a native reader against one installed SDK and prepare a hashed sample pack.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomLearningPackInspector LANGUAGES C)
if(NOT DEFINED UMICOM_EXPECTED_SDK_PREFIX OR
   NOT IS_ABSOLUTE "${UMICOM_EXPECTED_SDK_PREFIX}" OR
   NOT IS_DIRECTORY "${UMICOM_EXPECTED_SDK_PREFIX}")
    message(FATAL_ERROR "Supply UMICOM_EXPECTED_SDK_PREFIX as the installed SDK's absolute directory")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS developer
    PATHS "${UMICOM_EXPECTED_SDK_PREFIX}" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
if(NOT DEFINED UmicomFramework_CONFIG OR NOT EXISTS "${UmicomFramework_CONFIG}")
    message(FATAL_ERROR "The selected Framework configuration is not an existing file")
endif()
file(REAL_PATH "${UMICOM_EXPECTED_SDK_PREFIX}" _sdk_prefix)
file(REAL_PATH "${UmicomFramework_CONFIG}" _sdk_configuration)
cmake_path(IS_PREFIX _sdk_prefix "${_sdk_configuration}" NORMALIZE _sdk_inside)
if(NOT _sdk_inside)
    message(FATAL_ERROR "The selected Framework package is outside the requested prefix")
endif()
if(NOT COMMAND umicom_require_sdk_environment)
    message(FATAL_ERROR "Reinstall Framework with its SDK environment metadata")
endif()
umicom_require_sdk_environment(2)
umicom_require_installed_sdk_layout("${UMICOM_EXPECTED_SDK_PREFIX}" _headers Umicom::developer)
add_executable(umicom-learning-pack-inspect main.c)
target_link_libraries(umicom-learning-pack-inspect PRIVATE Umicom::developer)
set_target_properties(umicom-learning-pack-inspect PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(umicom-learning-pack-inspect PRIVATE /W4)
else()
    target_compile_options(umicom-learning-pack-inspect PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

# Hash the staged bytes, not an assumed newline representation from a different
# checkout. COPYONLY retains the bytes that the inspector will actually read.
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pack")
configure_file(guide.html "${CMAKE_CURRENT_BINARY_DIR}/pack/guide.html" COPYONLY)
configure_file(main.c "${CMAKE_CURRENT_BINARY_DIR}/pack/main.c" COPYONLY)
configure_file(history.c "${CMAKE_CURRENT_BINARY_DIR}/pack/history.c" COPYONLY)
configure_file(practice.c "${CMAKE_CURRENT_BINARY_DIR}/pack/practice.c" COPYONLY)
file(SHA256 "${CMAKE_CURRENT_BINARY_DIR}/pack/practice.c" _practice_hash)
file(SHA256 "${CMAKE_CURRENT_BINARY_DIR}/pack/guide.html" _guide_hash)
file(SHA256 "${CMAKE_CURRENT_BINARY_DIR}/pack/main.c" _example_hash)
file(SHA256 "${CMAKE_CURRENT_BINARY_DIR}/pack/history.c" _history_hash)
configure_file(learning-pack.json.in "${CMAKE_CURRENT_BINARY_DIR}/pack/learning-pack.json" @ONLY)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.learning_pack.list
        COMMAND umicom-learning-pack-inspect "${CMAKE_CURRENT_BINARY_DIR}/pack" learning-pack.json)
    foreach(_resource guide inspector reader-history practice-copy)
        add_test(NAME "sdk.learning_pack.verify.${_resource}"
            COMMAND umicom-learning-pack-inspect "${CMAKE_CURRENT_BINARY_DIR}/pack"
                learning-pack.json "${_resource}")
        set_tests_properties("sdk.learning_pack.verify.${_resource}" PROPERTIES TIMEOUT 30)
    endforeach()
    add_test(NAME sdk.learning_pack.navigation.example
        COMMAND umicom-learning-pack-inspect "${CMAKE_CURRENT_BINARY_DIR}/pack"
            learning-pack.json guide main.c)
    add_test(NAME sdk.learning_pack.navigation.anchor
        COMMAND umicom-learning-pack-inspect "${CMAKE_CURRENT_BINARY_DIR}/pack"
            learning-pack.json guide "#navigation")
    set_tests_properties(sdk.learning_pack.list sdk.learning_pack.navigation.example
        sdk.learning_pack.navigation.anchor PROPERTIES TIMEOUT 30)
endif()

# A separate small executable demonstrates reader-owned history without a GUI.
add_executable(umicom-learning-pack-history history.c)
target_link_libraries(umicom-learning-pack-history PRIVATE Umicom::developer)
set_target_properties(umicom-learning-pack-history PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(umicom-learning-pack-history PRIVATE /W4)
else()
    target_compile_options(umicom-learning-pack-history PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
if(BUILD_TESTING)
    add_test(NAME sdk.learning_pack.reader.history
        COMMAND umicom-learning-pack-history "${CMAKE_CURRENT_BINARY_DIR}/pack"
            learning-pack.json guide main.c)
    set_tests_properties(sdk.learning_pack.reader.history PROPERTIES TIMEOUT 30)
endif()

# Preview never writes; --copy selects the shared exclusive creation service.
add_executable(umicom-learning-pack-practice practice.c)
target_link_libraries(umicom-learning-pack-practice PRIVATE Umicom::developer)
set_target_properties(umicom-learning-pack-practice PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(umicom-learning-pack-practice PRIVATE /W4)
else()
    target_compile_options(umicom-learning-pack-practice PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
if(BUILD_TESTING)
    add_test(NAME sdk.learning_pack.practice.copy
        COMMAND "${CMAKE_COMMAND}"
            "-DPROGRAM=$<TARGET_FILE:umicom-learning-pack-practice>"
            "-DPACK_ROOT=${CMAKE_CURRENT_BINARY_DIR}/pack"
            "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/check_practice.cmake")
    set_tests_properties(sdk.learning_pack.practice.copy PROPERTIES TIMEOUT 120)
endif()
