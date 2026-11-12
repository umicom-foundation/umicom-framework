#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCompilerCppConsumer.cmake
#
# PURPOSE:
#   Register opt-in C++ consumers of the C compiler SDK, with independent header translation units.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# The caller explicitly enables CXX. Keeping qualification opt-in avoids adding
# a C++ compiler dependency to ordinary C23 Framework builds.
function(umicom_add_compiler_cpp_checks target_prefix include_root library_target)
    if(NOT CMAKE_CXX_COMPILER_LOADED)
        message(FATAL_ERROR "Enable CXX before registering compiler C++ consumers")
    endif()
    set(contracts
        abi|umi_compiler_abi_init
        artifact|umi_compiler_artifact_store_add
        assembly_provider|umi_compiler_assembly_provider
        build_graph|umi_compiler_build_graph_add
        cache|umi_compiler_request_hash
        capability|umi_compiler_capability_capture
        centre|umi_compiler_centre_init
        clang_provider|umi_compiler_clang_provider
        compilation_database|umi_polyglot_compilation_database_add
        diagnostic|umi_compiler_diagnostic_parse_line
        gcc_provider|umi_compiler_gcc_provider
        interop_graph|umi_interop_graph_add_node
        language|umi_compiler_language_name
        linker_profile|umi_linker_profile_init
        lockfile|umi_compiler_manifest_hash
        manifest|umi_compiler_manifest_init
        msvc_provider|umi_compiler_msvc_provider
        plan|umi_compiler_plan_init
        policy|umi_compiler_policy_debug
        profile|umi_compiler_profile_init
        provider|umi_compiler_provider_init
        registry|umi_compiler_registry_init
        request|umi_compiler_request_init
        response_file|umi_compiler_response_file_encode
        runtime_library|umi_compiler_runtime_register
        rust_provider|umi_compiler_rust_provider
        sysroot|umi_compiler_sysroot_register
        target|umi_compiler_target_parse
        toolchain_bridge|umi_compiler_registry_import_toolchain
        uai_provider|umi_compiler_uai_provider
        umicc_provider|umi_compiler_umicc_provider
        zig_provider|umi_compiler_zig_provider
    )
    set(check_targets)
    foreach(contract IN LISTS contracts)
        string(REPLACE "|" ";" parts "${contract}")
        list(GET parts 0 header)
        list(GET parts 1 symbol)
        set(target "${target_prefix}-header-${header}")
        add_executable("${target}"
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/test_compiler_cpp_header.cpp")
        if(NOT "${include_root}" STREQUAL "")
            target_include_directories("${target}" PRIVATE "${include_root}")
        endif()
        target_compile_definitions("${target}" PRIVATE
            "UMICOM_CPP_HEADER=\"umicom/compiler/${header}.h\""
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
        add_test(NAME "framework.compiler.cpp_header.${header}" COMMAND "${target}")
        set_tests_properties("framework.compiler.cpp_header.${header}" PROPERTIES
            TIMEOUT 30 LABELS "compiler;cpp-consumer;header-linkage")
        list(APPEND check_targets "${target}")
    endforeach()
    set(target "${target_prefix}-workflow")
    add_executable("${target}"
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/test_compiler_cpp_workflow.cpp")
    if(NOT "${include_root}" STREQUAL "")
        target_include_directories("${target}" PRIVATE "${include_root}")
    endif()
    target_link_libraries("${target}" PRIVATE "${library_target}")
    set_target_properties("${target}" PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings("${target}")
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers("${target}")
    endif()
    add_test(NAME framework.compiler.cpp_workflow COMMAND "${target}")
    set_tests_properties(framework.compiler.cpp_workflow PROPERTIES
        TIMEOUT 30 LABELS "compiler;cpp-consumer;planning")
    list(APPEND check_targets "${target}")
    add_custom_target("${target_prefix}-checks" DEPENDS ${check_targets})
endfunction()
