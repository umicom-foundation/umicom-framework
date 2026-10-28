#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/test_reuse/driver/CMakeLists.txt
#
# PURPOSE:
#   Provide a small native project for automatic qualification lifecycle checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.29)
project(AutomaticQualificationFixture LANGUAGES C)
enable_testing()
foreach(_case alpha beta unknown)
    add_executable("fixture-${_case}" "${_case}.c")
    set_target_properties("fixture-${_case}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES)
    add_test(NAME "fixture.${_case}" COMMAND "fixture-${_case}")
endforeach()
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/umicom/closed-tests-$<CONFIG>.tsv"
    CONTENT "fixture.alpha\t$<TARGET_FILE:fixture-alpha>\nfixture.beta\t$<TARGET_FILE:fixture-beta>\n")
set(_driver "set(UMICOM_AUTOMATIC_BUILD_ROOT [==[${CMAKE_BINARY_DIR}]==])\n")
string(APPEND _driver "set(UMICOM_AUTOMATIC_SOURCE_ROOT [==[${CMAKE_SOURCE_DIR}]==])\n")
string(APPEND _driver "set(UMICOM_AUTOMATIC_CTEST [==[${CMAKE_CTEST_COMMAND}]==])\n")
foreach(_key RECEIPTS SCRIPT COMPILER_DIR OBJDUMP RUNTIME_PLATFORM)
    string(APPEND _driver "set(UMICOM_AUTOMATIC_${_key} [==[${UMICOM_AUTOMATIC_${_key}}]==])\n")
endforeach()
string(APPEND _driver "include([==[${UMICOM_AUTOMATIC_SCRIPT}]==])\n")
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/umicom/automatic-tests-$<CONFIG>.cmake" CONTENT "${_driver}")
