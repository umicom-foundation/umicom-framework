#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/provider_text/CMakeLists.txt
#
# PURPOSE:
#   Check provider UTF-8 contracts using the actual codecs and native JSON owners.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomProviderTextChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    add_library(umicom_provider_text_subject STATIC
        "${_root}/src/provider_chat/codec.c"
        "${_root}/src/media_generation/heygen_request.c"
        "${_root}/src/ai/mcp/json.c"
        "${_root}/src/protocol/json_writer.c"
        "${_root}/src/language_runtime/json.c"
        "${_root}/src/language_runtime/json_tree.c"
        "${_root}/src/language_runtime/json_document.c"
        "${_root}/src/language_runtime/json_text.c"
        "${_root}/src/base/utf8.c"
        "${_root}/src/base/ascii_number.c"
        "${_root}/src/security/secrets.c"
        "${_root}/src/platform/cancellation.c")
    target_include_directories(umicom_provider_text_subject PUBLIC "${_root}/include")
    set_target_properties(umicom_provider_text_subject PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(NOT MSVC)
        target_compile_options(umicom_provider_text_subject PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    set(_subject umicom_provider_text_subject)
else()
    set(_subject Umicom::provider_chat Umicom::heygen)
endif()
add_executable(umicom-provider-text-test test_provider_text.c)
target_link_libraries(umicom-provider-text-test PRIVATE ${_subject})
set_target_properties(umicom-provider-text-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(NOT MSVC)
    target_compile_options(umicom-provider-text-test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-provider-text-test)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-provider-text-test)
endif()
foreach(_case chat-prompt chat-context script name motion valid fields)
    add_test(NAME framework.provider_text.${_case} COMMAND umicom-provider-text-test "${_case}")
    set_tests_properties(framework.provider_text.${_case} PROPERTIES TIMEOUT 20
        LABELS "framework;provider;utf8;regression;no-network")
endforeach()
