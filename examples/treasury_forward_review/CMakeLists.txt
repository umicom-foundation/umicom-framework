# Umicom Framework
# File: examples/treasury_forward_review/CMakeLists.txt
# PURPOSE: Exercise treasury transition reviews from one explicit installed SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomTreasuryForwardReviewConsumer LANGUAGES C)
if(NOT IS_ABSOLUTE "${UMICOM_SDK_PREFIX}" OR NOT IS_DIRECTORY "${UMICOM_SDK_PREFIX}")
    message(FATAL_ERROR "Set UMICOM_SDK_PREFIX to an absolute installed Framework prefix")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS finance
    PATHS "${UMICOM_SDK_PREFIX}" NO_DEFAULT_PATH)
umicom_require_installed_sdk("${UMICOM_SDK_PREFIX}" Umicom::finance Umicom::base)
add_executable(treasury-forward-review-consumer main.c)
target_link_libraries(treasury-forward-review-consumer PRIVATE Umicom::finance)
set_target_properties(treasury-forward-review-consumer PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(treasury-forward-review-consumer PRIVATE /W4)
else()
    target_compile_options(treasury-forward-review-consumer PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    # The earlier apply/stale cases remain; archive cases also exercise SDK closure.
    foreach(scenario apply stale archive-roundtrip archive-stale)
        add_test(NAME installed.treasury_forward_review.${scenario}
            COMMAND treasury-forward-review-consumer ${scenario})
        set_tests_properties(installed.treasury_forward_review.${scenario} PROPERTIES TIMEOUT 30)
    endforeach()
endif()
