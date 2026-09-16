#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/input_validation/CMakeLists.txt
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
    project(UmicomInputValidation LANGUAGES C)
    enable_testing()
    set(UMICOM_INPUT_VALIDATION_STANDALONE ON)
else()
    set(UMICOM_INPUT_VALIDATION_STANDALONE OFF)
endif()

# A ZIP overlay alone cannot remove the previous directory. Stop rather than
# leaving two competing copies available to source discovery or future builds.
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../studio_trader_contracts/CMakeLists.txt" OR
   EXISTS "${CMAKE_CURRENT_LIST_DIR}/../studio_trader_contracts/test_input_validation.c")
    message(FATAL_ERROR "Retired tests/studio_trader_contracts files remain. Perform the documented git mv migration before building tests/input_validation.")
endif()

# Avoid duplicate targets when a parent also registers this source directory.
if(TARGET umicom-input-validation-tests)
    return()
endif()

add_executable(umicom-input-validation-tests test_input_validation.c)
set_target_properties(umicom-input-validation-tests PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

if(UMICOM_INPUT_VALIDATION_STANDALONE)
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
    add_library(umicom-input-validation-subjects STATIC
        ${UMICOM_CONTRACT_SUBJECT_SOURCES})
    set_target_properties(umicom-input-validation-subjects PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_include_directories(umicom-input-validation-subjects PUBLIC
        "${UMICOM_CONTRACT_FRAMEWORK_ROOT}/include")
    target_link_libraries(umicom-input-validation-tests PRIVATE
        umicom-input-validation-subjects)

    option(UMICOM_CONTRACT_SANITIZERS "Use AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
    foreach(target IN ITEMS umicom-input-validation-subjects
                            umicom-input-validation-tests)
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
        target_link_options(umicom-input-validation-tests PRIVATE
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
    target_link_libraries(umicom-input-validation-tests PRIVATE
        Umicom::editor Umicom::developer Umicom::trading Umicom::finance)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-input-validation-tests)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-input-validation-tests)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-input-validation-tests)
    endif()
endif()

set(UMICOM_INPUT_VALIDATION_GROUPS
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
foreach(group IN LISTS UMICOM_INPUT_VALIDATION_GROUPS)
    add_test(NAME "framework.input_validation.${group}"
        COMMAND umicom-input-validation-tests "${group}")
    set_tests_properties("framework.input_validation.${group}" PROPERTIES
        LABELS "framework;regression;contracts;input-validation" TIMEOUT 30)
endforeach()
