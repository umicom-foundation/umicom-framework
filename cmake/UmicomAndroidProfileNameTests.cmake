#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAndroidProfileNameTests.cmake
#
# PURPOSE:
#   Register optional host contract checks against actual JNI declarations, separate from Android device qualification.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

option(UMICOM_BUILD_ANDROID_JNI_TESTS "Compile host JNI adapter contracts using an installed JDK" OFF)
if(NOT UMICOM_BUILD_ANDROID_JNI_TESTS)
    return()
endif()
if(CMAKE_CROSSCOMPILING OR ANDROID)
    message(FATAL_ERROR "Host JNI contracts need a native host build; Android instrumented tests run separately")
endif()
find_package(JNI REQUIRED COMPONENTS JVM)
# Only declarations are used: the controlled function table does not load libjvm.
add_executable(umicom_android_profile_name_test
    "${CMAKE_CURRENT_LIST_DIR}/../tests/android_profile_name/test_profile_name.c"
    "${CMAKE_CURRENT_LIST_DIR}/../adapters/android/profile_name.c")
set_target_properties(umicom_android_profile_name_test PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_android_profile_name_test PRIVATE ${JNI_INCLUDE_DIRS})
target_link_libraries(umicom_android_profile_name_test PRIVATE Umicom::profile_name)
foreach(_case IN ITEMS valid minimum maximum long empty short negative-length null-input
        first-digit space embedded-nul non-ascii surrogate length-exception region-exception
        new-exception new-null pending-exception null-environment)
    add_test(NAME "framework.android_profile_name.${_case}" COMMAND umicom_android_profile_name_test "${_case}")
    set_tests_properties("framework.android_profile_name.${_case}" PROPERTIES
        TIMEOUT 10 LABELS "android;profile;host-contract")
endforeach()
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_android_profile_name_test)
endif()
