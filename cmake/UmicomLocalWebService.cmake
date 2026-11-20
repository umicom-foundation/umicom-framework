#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLocalWebService.cmake
#
# PURPOSE:
#   Configure the reusable native loopback service and its bounded previews.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomEmbeddedBrandResources.cmake")
option(UMICOM_LOCAL_WEB_CVM_PREVIEW "Enable native Umicom C artifact previews in the local demo service" OFF)

function(umicom_add_local_web_cvm_preview)
    if(TARGET umicom_local_web_cvm_preview)
        return()
    endif()
    include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/UmicomCPreview.cmake")
    umicom_add_cvm_preview(umicom_local_web_cvm_preview)
    target_sources(umicom_local_web_cvm_preview PRIVATE
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../src/web/cvm_preview.c")
    target_link_libraries(umicom_local_web_cvm_preview PUBLIC Umicom::web)
    if(BUILD_TESTING)
        add_executable(umicom-web-cvm-preview-test
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/web/test_cvm_preview.c")
        target_link_libraries(umicom-web-cvm-preview-test PRIVATE umicom_local_web_cvm_preview)
        set_target_properties(umicom-web-cvm-preview-test PROPERTIES C_EXTENSIONS OFF)
        umicom_apply_warnings(umicom-web-cvm-preview-test)
        umicom_apply_sanitizers(umicom-web-cvm-preview-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-web-cvm-preview-test)
        endif()
        foreach(case binary uppercase permissions route query bad-media duplicate encoded empty
                     oversized truncated runtime fuel invalid-context)
            set(name "framework.web.cvm_preview.${case}")
            add_test(NAME "${name}" COMMAND umicom-web-cvm-preview-test "${case}")
            set_tests_properties("${name}" PROPERTIES TIMEOUT 20 LABELS "framework;web;cvm;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners("${name}" TARGETS umicom-web-cvm-preview-test)
            endif()
        endforeach()

        add_executable(umicom-web-cvm-int-input-test
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/web/test_cvm_int_input.c")
        target_link_libraries(umicom-web-cvm-int-input-test PRIVATE umicom_local_web_cvm_preview)
        set_target_properties(umicom-web-cvm-int-input-test PROPERTIES C_EXTENSIONS OFF)
        umicom_apply_warnings(umicom-web-cvm-int-input-test)
        umicom_apply_sanitizers(umicom-web-cvm-int-input-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-web-cvm-int-input-test)
        endif()
        foreach(case sdk positive negative missing empty duplicate extra trailing decimal range encoded
                     malformed-field prefix ordinary old-route origin arithmetic fuel media adapter-stale)
            set(name "framework.web.cvm_int_input.${case}")
            add_test(NAME "${name}" COMMAND umicom-web-cvm-int-input-test "${case}")
            set_tests_properties("${name}" PROPERTIES TIMEOUT 20 SKIP_RETURN_CODE 77
                LABELS "framework;web;cvm;int-input;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners("${name}" TARGETS umicom-web-cvm-int-input-test)
            endif()
        endforeach()
    endif()
endfunction()

# Reuse the same complete C example in a standalone SDK build and Web Studio.
# The product selects a target name; it does not copy sockets or request logic.
function(umicom_add_local_web_service target)
    if(NOT TARGET Umicom::web OR TARGET "${target}")
        message(FATAL_ERROR "Local web service needs Umicom::web and an unused target name")
    endif()
    add_executable("${target}" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../examples/web/local_service.c")
    target_link_libraries("${target}" PRIVATE Umicom::web)
    # Compile approved artwork into the example so its built-in website works
    # from any working directory and makes no request to an outside host.
    get_filename_component(_framework "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    set(_brand "${_framework}/resources/brand")
    umicom_brand_resource_initializer("${_brand}/umicom-logo.svg" UMICOM_WEB_BRAND_WORDMARK_BYTES)
    umicom_brand_resource_initializer("${_brand}/umicom-icon.svg" UMICOM_WEB_BRAND_ICON_SVG_BYTES)
    umicom_brand_resource_initializer("${_brand}/umicom.ico" UMICOM_WEB_BRAND_ICON_ICO_BYTES)
    set(_generated "${CMAKE_CURRENT_BINARY_DIR}/generated/${target}")
    configure_file("${_framework}/examples/web/brand_resources.h.in"
        "${_generated}/umicom_web_brand_resources.h" @ONLY)
    target_include_directories("${target}" PRIVATE "${_generated}")
    target_compile_definitions("${target}" PRIVATE UMICOM_LOCAL_WEB_BRANDING=1)
    if(UMICOM_LOCAL_WEB_CVM_PREVIEW)
        umicom_add_local_web_cvm_preview()
        target_link_libraries("${target}" PRIVATE umicom_local_web_cvm_preview)
        target_compile_definitions("${target}" PRIVATE UMICOM_LOCAL_WEB_CVM_PREVIEW)
        # Serve the selected compiler's own browser modules. Older native SDKs
        # still provide the C preview routes when their package has no worker.
        get_target_property(_cvm_browser UmicomC::Compiler UMICOM_C_BROWSER_DIRECTORY)
        if(_cvm_browser AND EXISTS "${_cvm_browser}/worker.mjs" AND EXISTS "${_cvm_browser}/arithmetic.mjs")
            umicom_resource_initializer("${_cvm_browser}/worker.mjs" UMICOM_WEB_CVM_WORKER_BYTES)
            umicom_resource_initializer("${_cvm_browser}/arithmetic.mjs" UMICOM_WEB_CVM_ARITHMETIC_BYTES)
            configure_file("${_framework}/examples/web/cvm_browser_resources.h.in"
                "${_generated}/umicom_web_cvm_browser_resources.h" @ONLY)
            target_compile_definitions("${target}" PRIVATE UMICOM_LOCAL_WEB_CVM_BROWSER)
            if(BUILD_TESTING)
                # Exercise the assembled body fallback without binding a socket.
                set(_browser_test "${target}-browser-assets-test")
                add_executable("${_browser_test}" "${_framework}/tests/web/test_cvm_browser_assets.c")
                target_include_directories("${_browser_test}" PRIVATE "${_generated}")
                target_compile_definitions("${_browser_test}" PRIVATE
                    UMICOM_LOCAL_WEB_BRANDING=1 UMICOM_LOCAL_WEB_CVM_BROWSER)
                target_link_libraries("${_browser_test}" PRIVATE Umicom::web)
                set_target_properties("${_browser_test}" PROPERTIES
                    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
                umicom_apply_warnings("${_browser_test}")
                umicom_apply_sanitizers("${_browser_test}")
                set(_case "framework.web.${target}.browser_asset_routes")
                add_test(NAME "${_case}" COMMAND "${_browser_test}")
                set_tests_properties("${_case}" PROPERTIES TIMEOUT 20
                    LABELS "framework;web;cvm;browser;regression")
                if(COMMAND umicom_register_validation_target)
                    umicom_register_validation_target("${_browser_test}")
                endif()
                if(COMMAND umicom_test_impact_owners)
                    umicom_test_impact_owners("${_case}" TARGETS "${_browser_test}")
                endif()
            endif()
        else()
            message(STATUS "The selected Umicom C SDK supplies native CVM preview. Browser worker modules are unavailable.")
        endif()
    endif()
    set_target_properties("${target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings("${target}")
    umicom_apply_sanitizers("${target}")
    install(TARGETS "${target}" RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
endfunction()
if(UMICOM_BUILD_LEARNING_EXAMPLES)
    umicom_add_local_web_service(umicom-web-local-service-example)
endif()
