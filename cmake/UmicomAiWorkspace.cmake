#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiWorkspace.cmake
# PURPOSE: Build shared AI sources, reviewed jobs, optional local transport and thin application integration.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
include(GNUInstallDirs)
include(CMakeParseArguments)
set(_umicom_ai_workspace_root "${CMAKE_CURRENT_LIST_DIR}/..")
foreach(dependency IN ITEMS Umicom::ai Umicom::data)
    if(NOT TARGET ${dependency})
        message(FATAL_ERROR "AI workspace requires canonical ${dependency}")
    endif()
endforeach()

# AUTO preserves an offline build when transport libraries are absent. ON is
# an explicit requirement and fails configuration rather than substituting a
# mock model. The application also displays the compiled capability honestly.
set(UMICOM_AI_WORKSPACE_LOCAL_HTTP "AUTO" CACHE STRING "Local AI HTTP client: AUTO, ON or OFF")
set_property(CACHE UMICOM_AI_WORKSPACE_LOCAL_HTTP PROPERTY STRINGS AUTO ON OFF)
string(TOUPPER "${UMICOM_AI_WORKSPACE_LOCAL_HTTP}" _umicom_ai_http_mode)
if(NOT _umicom_ai_http_mode MATCHES "^(AUTO|ON|OFF)$")
    message(FATAL_ERROR "UMICOM_AI_WORKSPACE_LOCAL_HTTP must be AUTO, ON or OFF")
endif()
set(UMICOM_AI_WORKSPACE_HTTP_BUILT OFF)
if(NOT _umicom_ai_http_mode STREQUAL "OFF")
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(UMICOM_AI_LOCAL_CURL QUIET IMPORTED_TARGET GLOBAL libcurl>=7.85.0)
        pkg_check_modules(UMICOM_AI_LOCAL_JSONC QUIET IMPORTED_TARGET GLOBAL json-c>=0.15)
    endif()
    if(TARGET PkgConfig::UMICOM_AI_LOCAL_CURL AND TARGET PkgConfig::UMICOM_AI_LOCAL_JSONC)
        set(UMICOM_AI_WORKSPACE_HTTP_BUILT ON)
    elseif(_umicom_ai_http_mode STREQUAL "ON")
        message(FATAL_ERROR "Local AI HTTP requires pkg-config, libcurl >= 7.85 and json-c >= 0.15. Use OFF for an explicitly offline build.")
    endif()
endif()
message(STATUS "Umicom AI workspace local HTTP transport: ${UMICOM_AI_WORKSPACE_HTTP_BUILT}")

function(umicom_ai_workspace_configure_target target)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
endfunction()

add_library(umicom_ai_workspace STATIC
    "${_umicom_ai_workspace_root}/src/ai_workspace/cancellation.c"
    "${_umicom_ai_workspace_root}/src/ai_workspace/text.c"
    "${_umicom_ai_workspace_root}/src/ai_workspace/wire.c"
    "${_umicom_ai_workspace_root}/src/ai_workspace/storage.c"
    "${_umicom_ai_workspace_root}/src/ai_workspace/workspace.c"
    "${_umicom_ai_workspace_root}/src/ai_workspace/retrieval.c"
    "${_umicom_ai_workspace_root}/src/ai_workspace/jobs.c"
    "${_umicom_ai_workspace_root}/src/ai_workspace/request.c"
    "${_umicom_ai_workspace_root}/src/ai_workspace/extractive_provider.c"
    "${_umicom_ai_workspace_root}/src/ai_workspace/local_chat_codec.c"
    "${_umicom_ai_workspace_root}/src/ai_workspace/local_chat_provider.c")
add_library(Umicom::ai_workspace ALIAS umicom_ai_workspace)
set_target_properties(umicom_ai_workspace PROPERTIES EXPORT_NAME ai_workspace)
target_include_directories(umicom_ai_workspace PUBLIC
    $<BUILD_INTERFACE:${_umicom_ai_workspace_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_ai_workspace PUBLIC Umicom::ai Umicom::data)
if(UMICOM_AI_WORKSPACE_HTTP_BUILT)
    target_compile_definitions(umicom_ai_workspace PRIVATE UMICOM_AI_WORKSPACE_HAS_HTTP=1)
    target_link_libraries(umicom_ai_workspace PRIVATE
        PkgConfig::UMICOM_AI_LOCAL_CURL PkgConfig::UMICOM_AI_LOCAL_JSONC)
endif()
umicom_ai_workspace_configure_target(umicom_ai_workspace)
install(TARGETS umicom_ai_workspace EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(DIRECTORY "${_umicom_ai_workspace_root}/include/umicom/ai_workspace"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom" COMPONENT Framework)
configure_file("${CMAKE_CURRENT_LIST_DIR}/UmicomAiWorkspaceDependencies.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/umicom-ai-workspace-config/UmicomAiWorkspaceDependencies.cmake" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/umicom-ai-workspace-config/UmicomAiWorkspaceDependencies.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/UmicomFramework" COMPONENT Framework)

if(TARGET Umicom::ui_gtk4)
    add_library(umicom_ai_workspace_gtk4 STATIC
        "${_umicom_ai_workspace_root}/adapters/gtk4/ai_workspace_panel_gtk4.c"
        "${_umicom_ai_workspace_root}/adapters/gtk4/ai_workspace_application_gtk4.c")
    add_library(Umicom::ai_workspace_gtk4 ALIAS umicom_ai_workspace_gtk4)
    set_target_properties(umicom_ai_workspace_gtk4 PROPERTIES EXPORT_NAME ai_workspace_gtk4)
    target_link_libraries(umicom_ai_workspace_gtk4 PUBLIC Umicom::ai_workspace Umicom::ui_gtk4)
    umicom_ai_workspace_configure_target(umicom_ai_workspace_gtk4)
    install(TARGETS umicom_ai_workspace_gtk4 EXPORT UmicomFrameworkTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
    install(FILES "${_umicom_ai_workspace_root}/include/umicom/ui/gtk4/ai_workspace.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
endif()

if(BUILD_TESTING)
    add_executable(umicom-ai-workspace-test "${_umicom_ai_workspace_root}/tests/ai_workspace/test_workspace.c")
    target_link_libraries(umicom-ai-workspace-test PRIVATE Umicom::ai_workspace)
    target_include_directories(umicom-ai-workspace-test PRIVATE "${_umicom_ai_workspace_root}/src/ai_workspace")
    umicom_ai_workspace_configure_target(umicom-ai-workspace-test)
    include("${_umicom_ai_workspace_root}/tests/ai_workspace/cases.cmake")
    foreach(case IN LISTS UMICOM_AI_WORKSPACE_CASES UMICOM_AI_WORKSPACE_SQLITE_CASES)
        add_test(NAME "framework.ai_workspace.${case}" COMMAND umicom-ai-workspace-test "${case}"
            "${CMAKE_CURRENT_BINARY_DIR}/ai-workspace-${case}.sqlite")
        set_tests_properties("framework.ai_workspace.${case}" PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77
            LABELS "framework;ai-workspace;regression")
    endforeach()
    add_executable(umicom-ai-workspace-codec-test "${_umicom_ai_workspace_root}/tests/ai_workspace/test_local_chat_codec.c")
    target_link_libraries(umicom-ai-workspace-codec-test PRIVATE Umicom::ai_workspace)
    target_include_directories(umicom-ai-workspace-codec-test PRIVATE "${_umicom_ai_workspace_root}/src/ai_workspace")
    umicom_ai_workspace_configure_target(umicom-ai-workspace-codec-test)
    include("${_umicom_ai_workspace_root}/tests/ai_workspace/protocol_cases.cmake")
    foreach(case IN LISTS UMICOM_AI_WORKSPACE_PROTOCOL_CASES)
        add_test(NAME "framework.ai_workspace.codec.${case}" COMMAND umicom-ai-workspace-codec-test "${case}")
        set_tests_properties("framework.ai_workspace.codec.${case}" PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
            LABELS "framework;ai-workspace;protocol;regression")
    endforeach()
    find_package(Threads REQUIRED)
    add_executable(umicom-ai-workspace-http-client "${_umicom_ai_workspace_root}/tests/ai_workspace/test_local_chat_client.c")
    target_link_libraries(umicom-ai-workspace-http-client PRIVATE Umicom::ai_workspace Threads::Threads)
    if(NOT WIN32)
        target_compile_definitions(umicom-ai-workspace-http-client PRIVATE _POSIX_C_SOURCE=200809L)
    endif()
    umicom_ai_workspace_configure_target(umicom-ai-workspace-http-client)
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        foreach(case success proxy_bypass redirect_blocked busy bad_json oversized timeout cancel tool_call model_mismatch)
            add_test(NAME "framework.ai_workspace.http.${case}" COMMAND "${Python3_EXECUTABLE}"
                "${_umicom_ai_workspace_root}/tests/ai_workspace/test_local_chat_http.py"
                "$<TARGET_FILE:umicom-ai-workspace-http-client>" "${case}")
            set_tests_properties("framework.ai_workspace.http.${case}" PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
                LABELS "framework;ai-workspace;loopback;regression")
        endforeach()
    else()
        message(STATUS "AI loopback HTTP fixture tests are not registered: Python interpreter unavailable")
    endif()
    if(TARGET Umicom::ai_workspace_gtk4)
        add_executable(umicom-ai-workspace-gtk-test "${_umicom_ai_workspace_root}/tests/ai_workspace/test_panel_gtk4.c")
        target_link_libraries(umicom-ai-workspace-gtk-test PRIVATE Umicom::ai_workspace_gtk4)
        umicom_ai_workspace_configure_target(umicom-ai-workspace-gtk-test)
        foreach(case workflow close_during_request)
            add_test(NAME "framework.ai_workspace.gtk.${case}" COMMAND umicom-ai-workspace-gtk-test "${case}")
            set_tests_properties("framework.ai_workspace.gtk.${case}" PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
                LABELS "framework;ai-workspace;gtk4;regression")
        endforeach()
    endif()
    foreach(target umicom-ai-workspace-test umicom-ai-workspace-codec-test umicom-ai-workspace-http-client umicom-ai-workspace-gtk-test)
        if(TARGET ${target} AND COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endforeach()
endif()

# Only composition and example identity live in the product repositories.
function(umicom_attach_ai_workspace)
    cmake_parse_arguments(PARSE_ARGV 0 UMI "" "NAME;TARGET" "")
    if(UMI_UNPARSED_ARGUMENTS OR NOT UMI_NAME OR NOT UMI_TARGET OR NOT TARGET "${UMI_TARGET}")
        message(FATAL_ERROR "AI workspace attachment requires NAME and an existing TARGET")
    endif()
    get_target_property(attached "${UMI_TARGET}" UMICOM_AI_WORKSPACE_ATTACHED)
    if(attached)
        message(FATAL_ERROR "AI workspace is already attached to ${UMI_TARGET}")
    endif()
    set_target_properties("${UMI_TARGET}" PROPERTIES UMICOM_AI_WORKSPACE_ATTACHED TRUE)
    target_link_libraries("${UMI_TARGET}" PUBLIC Umicom::ai_workspace)
    set(example "umicom-${UMI_NAME}-ai-workspace-example")
    add_executable("${example}" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../examples/ai_workspace_main.c")
    target_link_libraries("${example}" PRIVATE Umicom::ai_workspace)
    umicom_ai_workspace_configure_target("${example}")
    install(TARGETS "${example}" RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
    if(BUILD_TESTING)
        add_test(NAME "${UMI_NAME}.ai_workspace.example" COMMAND "${example}")
        set_tests_properties("${UMI_NAME}.ai_workspace.example" PROPERTIES TIMEOUT 30 LABELS "${UMI_NAME};ai-workspace;example")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${example}")
        endif()
    endif()
endfunction()
install(FILES "${_umicom_ai_workspace_root}/docs/learning/AI_WORKSPACE.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)
unset(_umicom_ai_workspace_root)
