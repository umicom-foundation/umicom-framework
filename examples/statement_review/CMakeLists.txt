# Umicom Framework
# File: examples/statement_review/CMakeLists.txt
# PURPOSE: Consume a reviewed statement archive from one explicit installed SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomStatementReviewConsumer LANGUAGES C)
if(NOT IS_ABSOLUTE "${UMICOM_SDK_PREFIX}" OR NOT IS_DIRECTORY "${UMICOM_SDK_PREFIX}")
    message(FATAL_ERROR "Set UMICOM_SDK_PREFIX to an absolute installed Framework prefix")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS bank_feed
    PATHS "${UMICOM_SDK_PREFIX}" NO_DEFAULT_PATH)
umicom_require_installed_sdk("${UMICOM_SDK_PREFIX}" Umicom::bank_feed Umicom::finance Umicom::base)
add_executable(statement-review-consumer main.c)
target_link_libraries(statement-review-consumer PRIVATE Umicom::bank_feed)
set_target_properties(statement-review-consumer PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(statement-review-consumer PRIVATE /W4)
else()
    target_compile_options(statement-review-consumer PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    foreach(scenario roundtrip mismatch)
        add_test(NAME installed.statement_review.${scenario} COMMAND statement-review-consumer ${scenario})
        set_tests_properties(installed.statement_review.${scenario} PROPERTIES TIMEOUT 30)
    endforeach()
endif()
