#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/local_mfa/CMakeLists.txt
#
# PURPOSE:
#   Qualify password, authenticator replay and session composition from the
#   same production sources in integrated and focused builds.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomLocalMfaChecks LANGUAGES C)
    include(CTest)
    find_package(Threads REQUIRED)
    get_filename_component(_framework "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
    add_library(umicom-local-mfa-check-support STATIC
        "${_framework}/src/security/local_profile_mfa.c"
        "${_framework}/src/security/local_profile.c"
        "${_framework}/src/security/profile_name.c"
        "${_framework}/src/security/secrets.c"
        "${_framework}/src/security/session.c"
        "${_framework}/src/security/token.c"
        "${_framework}/src/security/totp.c"
        "${_framework}/src/security/totp_platform.c"
        "${_framework}/src/platform/secure_random.c"
        "${_framework}/src/platform/threading.c")
    target_include_directories(umicom-local-mfa-check-support PUBLIC "${_framework}/include")
    target_compile_features(umicom-local-mfa-check-support PRIVATE c_std_23)
    target_link_libraries(umicom-local-mfa-check-support PRIVATE Threads::Threads)
    if(WIN32)
        target_link_libraries(umicom-local-mfa-check-support PRIVATE bcrypt)
    endif()
    set(_mfa_owner umicom-local-mfa-check-support)
else()
    set(_mfa_owner Umicom::security)
endif()

add_executable(umicom-local-mfa-test test_sign_in.c)
target_link_libraries(umicom-local-mfa-test PRIVATE ${_mfa_owner})
target_compile_features(umicom-local-mfa-test PRIVATE c_std_23)
set_target_properties(umicom-local-mfa-test PROPERTIES C_EXTENSIONS OFF)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-local-mfa-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-local-mfa-test)
endif()
foreach(_case sign-in password-refused code-refused malformed-code missing-credential replay corrupt-record
        concurrent-use rotation commit-io provider-error registry-full invalid-clock)
    set(_test "framework.local_profile_mfa.${_case}")
    add_test(NAME "${_test}" COMMAND umicom-local-mfa-test "${_case}")
    set_tests_properties("${_test}" PROPERTIES TIMEOUT 20 LABELS "framework;security;mfa;local-fixture")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("${_test}" TARGETS umicom-local-mfa-test)
    endif()
endforeach()

add_executable(umicom-local-mfa-enrollment-test test_confirm_enrollment.c)
target_link_libraries(umicom-local-mfa-enrollment-test PRIVATE ${_mfa_owner})
target_compile_features(umicom-local-mfa-enrollment-test PRIVATE c_std_23)
set_target_properties(umicom-local-mfa-enrollment-test PROPERTIES C_EXTENSIONS OFF)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-local-mfa-enrollment-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-local-mfa-enrollment-test)
endif()
foreach(_case activate activate-and-sign-in duplicate-confirmation rotate password-first wrong-code
        missing expired stale-ticket pending-race active-race write-failure invalid-record reused-pending)
    set(_test "framework.local_profile_mfa_enrollment.${_case}")
    add_test(NAME "${_test}" COMMAND umicom-local-mfa-enrollment-test "${_case}")
    set_tests_properties("${_test}" PROPERTIES TIMEOUT 20 LABELS "framework;security;mfa;local-fixture")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners("${_test}" TARGETS umicom-local-mfa-enrollment-test)
    endif()
endforeach()
