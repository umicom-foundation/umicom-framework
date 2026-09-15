#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/studio_trader_contracts/CMakeLists.txt
#
# PURPOSE:
#   Register the shared input-contract regressions in Applications, or compile
#   the exact production source subset as an isolated qualification project.
#   The standalone path is not a full Framework or Applications build.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.24)
    project(UmicomStudioTraderContracts LANGUAGES C)
    enable_testing()
    set(UMICOM_STUDIO_TRADER_STANDALONE ON)
else()
    set(UMICOM_STUDIO_TRADER_STANDALONE OFF)
endif()

# Avoid duplicate targets when a parent also registers this source directory.
if(TARGET umicom-studio-trader-contract-tests)
    return()
endif()

add_executable(umicom-studio-trader-contract-tests test_input_validation.c)
set_target_properties(umicom-studio-trader-contract-tests PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

if(UMICOM_STUDIO_TRADER_STANDALONE)
    # An explicit override is useful for running the same tests on an untouched
    # baseline checkout. Never select a replacement implementation or mock ABI.
    set(UMICOM_CONTRACT_FRAMEWORK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.."
        CACHE PATH "Framework source root for isolated contract qualification")
    get_filename_component(UMICOM_CONTRACT_FRAMEWORK_ROOT
        "${UMICOM_CONTRACT_FRAMEWORK_ROOT}" ABSOLUTE)
    set(UMICOM_CONTRACT_SUBJECT_SOURCES
        src/editor/document.c
        src/teacher/learning_plan.c
        src/finance/identifier.c
        src/finance/currency.c
        src/trading/instrument.c
        src/trading/order_request.c
        src/trading/quote.c
        src/trading/risk_limit.c
        src/trading/risk_context.c
        src/trading/risk_decision.c
        src/trading/pretrade_risk.c)
    list(TRANSFORM UMICOM_CONTRACT_SUBJECT_SOURCES
        PREPEND "${UMICOM_CONTRACT_FRAMEWORK_ROOT}/")
    foreach(source IN LISTS UMICOM_CONTRACT_SUBJECT_SOURCES)
        if(NOT EXISTS "${source}")
            message(FATAL_ERROR "Missing production contract source: ${source}")
        endif()
    endforeach()
    add_library(umicom-studio-trader-contract-subjects STATIC
        ${UMICOM_CONTRACT_SUBJECT_SOURCES})
    set_target_properties(umicom-studio-trader-contract-subjects PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_include_directories(umicom-studio-trader-contract-subjects PUBLIC
        "${UMICOM_CONTRACT_FRAMEWORK_ROOT}/include")
    target_link_libraries(umicom-studio-trader-contract-tests PRIVATE
        umicom-studio-trader-contract-subjects)

    option(UMICOM_CONTRACT_SANITIZERS "Use AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
    foreach(target IN ITEMS umicom-studio-trader-contract-subjects
                            umicom-studio-trader-contract-tests)
        if(CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
            target_compile_options(${target} PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            if(UMICOM_CONTRACT_SANITIZERS)
                target_compile_options(${target} PRIVATE
                    -fsanitize=address,undefined -fno-omit-frame-pointer)
            endif()
        elseif(MSVC)
            target_compile_options(${target} PRIVATE /W4 /WX)
        endif()
    endforeach()
    if(UMICOM_CONTRACT_SANITIZERS)
        if(NOT CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
            message(FATAL_ERROR "This sanitizer configuration requires GCC or Clang")
        endif()
        target_link_options(umicom-studio-trader-contract-tests PRIVATE
            -fsanitize=address,undefined)
    endif()
else()
    # In a real Applications build consume the existing Framework libraries.
    # Do not compile private copies of their validation code into either app.
    foreach(dependency IN ITEMS Umicom::editor Umicom::developer
                                Umicom::trading Umicom::finance)
        if(NOT TARGET "${dependency}")
            message(FATAL_ERROR "Shared contract tests require ${dependency}")
        endif()
    endforeach()
    target_link_libraries(umicom-studio-trader-contract-tests PRIVATE
        Umicom::editor Umicom::developer Umicom::trading Umicom::finance)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-trader-contract-tests)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-trader-contract-tests)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-studio-trader-contract-tests)
    endif()
endif()

set(UMICOM_STUDIO_TRADER_CONTRACT_GROUPS
    editor_metadata_insert
    editor_rejected_replacement
    financial_identifier_bounds
    instrument_validation
    learning_identifier_bounds
    learning_plan_integrity
    learning_navigation
    order_numeric_fields
    order_enumerations
    order_price_requirements
    quote_numeric_fields
    quote_arithmetic
    risk_input_validation
    risk_arithmetic)
foreach(group IN LISTS UMICOM_STUDIO_TRADER_CONTRACT_GROUPS)
    add_test(NAME "framework.studio_trader_contracts.${group}"
        COMMAND umicom-studio-trader-contract-tests "${group}")
    set_tests_properties("framework.studio_trader_contracts.${group}" PROPERTIES
        LABELS "framework;regression;contracts;input-validation" TIMEOUT 30)
endforeach()
