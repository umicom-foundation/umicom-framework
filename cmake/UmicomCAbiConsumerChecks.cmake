#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCAbiConsumerChecks.cmake
#
# PURPOSE:
#   Share isolated public-header C++ linkage checks across native SDK components.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------


include_guard(GLOBAL)

# Consumers explicitly opt into CXX; this helper never changes the language
# requirements of ordinary Framework builds or calls a product API at configure.
function(umicom_add_c_abi_header_check target test_name header symbol library_target include_root)
    if(NOT CMAKE_CXX_COMPILER_LOADED)
        message(FATAL_ERROR "Enable CXX before registering C ABI consumer checks")
    endif()
    if(NOT TARGET "${library_target}")
        message(FATAL_ERROR "C ABI consumer requires exported target ${library_target}")
    endif()
    add_executable("${target}"
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/sdk_consumers/c_abi_header.cpp")
    if(NOT "${include_root}" STREQUAL "")
        target_include_directories("${target}" PRIVATE "${include_root}")
    endif()
    target_compile_definitions("${target}" PRIVATE
        "UMICOM_CPP_HEADER=\"${header}\""
        "UMICOM_CPP_SYMBOL=${symbol}")
    target_link_libraries("${target}" PRIVATE "${library_target}")
    set_target_properties("${target}" PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings("${target}")
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers("${target}")
    endif()
    add_test(NAME "${test_name}" COMMAND "${target}")
    set_tests_properties("${test_name}" PROPERTIES
        TIMEOUT 30 LABELS "framework;sdk;cpp-consumer;header-linkage")
endfunction()
