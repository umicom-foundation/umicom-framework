#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCreativeWorkspace.cmake
#
# PURPOSE:
#   Compose shared creative services and thin native product profiles.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
# This module also supports independent composition; own its SDK registration dependency.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include_guard(GLOBAL)
include(GNUInstallDirs)
include(CMakeParseArguments)
set(_umicom_creative_root "${CMAKE_CURRENT_LIST_DIR}/..")
foreach(dependency IN ITEMS Umicom::data Umicom::media_engine)
    if(NOT TARGET ${dependency})
        message(FATAL_ERROR "Creative workspace requires canonical ${dependency}")
    endif()
endforeach()
# The shared consumer helper owns this policy in both SDK and source builds.
# Retain the original definition for architectural review.
if(FALSE)
function(umicom_creative_configure_target target)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
endfunction()
endif()
include("${CMAKE_CURRENT_LIST_DIR}/UmicomCreativeApplication.cmake")
add_library(umicom_creative_workspace STATIC
    "${_umicom_creative_root}/src/creative_workspace/brand_assets.c"
    "${_umicom_creative_root}/src/creative_workspace/export_file.c"
    "${_umicom_creative_root}/src/creative_workspace/export_html.c"
    "${_umicom_creative_root}/src/creative_workspace/export_svg.c"
    "${_umicom_creative_root}/src/creative_workspace/export_wave.c"
    "${_umicom_creative_root}/src/creative_workspace/export_writer.c"
    "${_umicom_creative_root}/src/creative_workspace/motion.c"
    "${_umicom_creative_root}/src/creative_workspace/project.c"
    "${_umicom_creative_root}/src/creative_workspace/scene.c"
    "${_umicom_creative_root}/src/creative_workspace/starters.c"
    "${_umicom_creative_root}/src/creative_workspace/storage.c"
    "${_umicom_creative_root}/src/creative_workspace/text.c"
    "${_umicom_creative_root}/src/creative_workspace/validation.c"
    "${_umicom_creative_root}/src/creative_workspace/wire.c"
    "${_umicom_creative_root}/src/creative_workspace/workspace.c"
 )
add_library(Umicom::creative_workspace ALIAS umicom_creative_workspace)
# Keep offline documents independent of the runtime and filesystem by
# compiling the approved artwork into the existing shared export library.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomEmbeddedBrandResources.cmake")
umicom_brand_resource_initializer("${_umicom_creative_root}/resources/brand/umicom-logo-on-dark.svg"
    UMICOM_CREATIVE_BRAND_WORDMARK_BYTES)
umicom_brand_resource_initializer("${_umicom_creative_root}/resources/brand/umicom.ico"
    UMICOM_CREATIVE_BRAND_ICON_BYTES)
set(_umicom_creative_brand_generated "${CMAKE_CURRENT_BINARY_DIR}/generated/creative_brand")
configure_file("${_umicom_creative_root}/src/creative_workspace/brand_resources.h.in"
    "${_umicom_creative_brand_generated}/umicom_creative_brand_resources.h" @ONLY)
target_include_directories(umicom_creative_workspace PRIVATE "${_umicom_creative_brand_generated}")
set_target_properties(umicom_creative_workspace PROPERTIES EXPORT_NAME creative_workspace)
target_include_directories(umicom_creative_workspace PUBLIC
    $<BUILD_INTERFACE:${_umicom_creative_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_creative_workspace PUBLIC Umicom::data Umicom::media_engine)
if(TARGET Umicom::base)
    target_link_libraries(umicom_creative_workspace PRIVATE Umicom::base)
else()
    # The standalone audio SDK composes a small subset without a base target.
    # Compile the canonical encoder there instead of carrying a second implementation.
    target_sources(umicom_creative_workspace PRIVATE "${_umicom_creative_root}/src/base/base64.c")
endif()
if(NOT WIN32)
    target_link_libraries(umicom_creative_workspace PUBLIC m)
endif()
umicom_creative_configure_target(umicom_creative_workspace)
install(TARGETS umicom_creative_workspace EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
# Include this exported library surface in the opt-in SDK without changing full installs.
umicom_register_sdk_libraries(
    umicom_creative_workspace
)
install(DIRECTORY "${_umicom_creative_root}/include/umicom/creative_workspace"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom" COMPONENT Framework)
if(TARGET Umicom::ui_gtk4)
    # Both hosts use the same explicit local library selection and file policy.
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomWorkspaceLibrary.cmake")
    add_library(umicom_creative_workspace_gtk4 STATIC
        "${_umicom_creative_root}/adapters/gtk4/creative_workspace_panel_gtk4.c"
        "${_umicom_creative_root}/adapters/gtk4/creative_workspace_commands_gtk4.c"
        "${_umicom_creative_root}/adapters/gtk4/creative_workspace_canvas_gtk4.c"
        "${_umicom_creative_root}/adapters/gtk4/creative_workspace_surface_gtk4.c"
        "${_umicom_creative_root}/adapters/gtk4/creative_workspace_application_gtk4.c")
    add_library(Umicom::creative_workspace_gtk4 ALIAS umicom_creative_workspace_gtk4)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomCreativeMediaResources.cmake")
    target_link_libraries(umicom_creative_workspace_gtk4 PUBLIC Umicom::workspace_library_gtk4)
    set_target_properties(umicom_creative_workspace_gtk4 PROPERTIES EXPORT_NAME creative_workspace_gtk4)
    target_link_libraries(umicom_creative_workspace_gtk4 PUBLIC Umicom::creative_workspace Umicom::ui_gtk4)
    umicom_creative_configure_target(umicom_creative_workspace_gtk4)
    install(TARGETS umicom_creative_workspace_gtk4 EXPORT UmicomFrameworkTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
    # Include this exported library surface in the opt-in SDK without changing full installs.
    umicom_register_sdk_libraries(
        umicom_creative_workspace_gtk4
    )
    install(FILES "${_umicom_creative_root}/include/umicom/ui/gtk4/creative_workspace.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
endif()
if(BUILD_TESTING)
    add_executable(umicom-creative-workspace-test
        "${_umicom_creative_root}/tests/creative_workspace/test_creative_workspace.c")
    target_include_directories(umicom-creative-workspace-test PRIVATE
        "${_umicom_creative_root}/src/creative_workspace")
    target_link_libraries(umicom-creative-workspace-test PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target(umicom-creative-workspace-test)
    include("${_umicom_creative_root}/tests/creative_workspace/cases.cmake")
    foreach(case IN LISTS UMICOM_CREATIVE_WORKSPACE_CASES)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/creative-tests/${case}")
        add_test(NAME "framework.creative_workspace.${case}"
            COMMAND umicom-creative-workspace-test "${case}")
        set_tests_properties("framework.creative_workspace.${case}" PROPERTIES
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/creative-tests/${case}"
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative-workspace;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-creative-workspace-test)
    endif()
    add_executable(umicom-creative-export-fixture "${_umicom_creative_root}/tests/creative_workspace/export_fixture.c")
    target_link_libraries(umicom-creative-export-fixture PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target(umicom-creative-export-fixture)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-creative-export-fixture)
    endif()
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        add_test(NAME framework.creative_workspace.independent_exports
            COMMAND "${Python3_EXECUTABLE}" "${_umicom_creative_root}/tests/creative_workspace/check_exports.py"
                $<TARGET_FILE:umicom-creative-export-fixture>)
        set_tests_properties(framework.creative_workspace.independent_exports PROPERTIES TIMEOUT 60 LABELS "creative-workspace;export;regression")
    else()
        message(STATUS "Creative workspace independent export check not registered: Python3 interpreter unavailable")
    endif()
    if(TARGET Umicom::creative_workspace_gtk4)
        add_executable(umicom-creative-workspace-gtk-test
            "${_umicom_creative_root}/tests/creative_workspace/test_panel_gtk4.c")
        target_link_libraries(umicom-creative-workspace-gtk-test PRIVATE Umicom::creative_workspace_gtk4)
        umicom_creative_configure_target(umicom-creative-workspace-gtk-test)
        add_executable(umicom-creative-library-gtk-test
            "${_umicom_creative_root}/tests/creative_workspace/test_library_gtk4.c")
        target_link_libraries(umicom-creative-library-gtk-test PRIVATE Umicom::creative_workspace_gtk4)
        umicom_creative_configure_target(umicom-creative-library-gtk-test)
        add_test(NAME framework.creative_workspace.gtk.library COMMAND umicom-creative-library-gtk-test)
        set_tests_properties(framework.creative_workspace.gtk.library PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative-workspace;gtk4;storage")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-creative-library-gtk-test)
        endif()
        foreach(case workflow retained_controls surface_lifetime music-notes music-duplicate music-retired)
            add_test(NAME "framework.creative_workspace.gtk.${case}"
                COMMAND umicom-creative-workspace-gtk-test "${case}")
            set_tests_properties("framework.creative_workspace.gtk.${case}" PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative-workspace;gtk4;regression")
        endforeach()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-creative-workspace-gtk-test)
        endif()
    endif()
endif()
# A product selects a profile rather than copying any creative service.
# Attachment now lives in the installable consumer helper above. This retained
# implementation documents the former producer-relative example location.
if(FALSE)
function(umicom_attach_creative_workspace)
    cmake_parse_arguments(PARSE_ARGV 0 UMI "" "NAME;TARGET" "")
    if(UMI_UNPARSED_ARGUMENTS OR NOT UMI_NAME OR NOT UMI_TARGET OR NOT TARGET "${UMI_TARGET}")
        message(FATAL_ERROR "Creative attachment requires NAME and an existing TARGET")
    endif()
    if(NOT UMI_NAME MATCHES "^(media|music|cad|kitchen|games|web-studio|mobile-studio)$")
        message(FATAL_ERROR "Unknown creative profile: ${UMI_NAME}")
    endif()
    get_target_property(attached "${UMI_TARGET}" UMICOM_CREATIVE_WORKSPACE_ATTACHED)
    if(attached)
        message(FATAL_ERROR "Creative workspace already attached to ${UMI_TARGET}")
    endif()
    set_target_properties("${UMI_TARGET}" PROPERTIES
        UMICOM_CREATIVE_WORKSPACE_ATTACHED TRUE UMICOM_CREATIVE_WORKSPACE_PROFILE "${UMI_NAME}")
    target_link_libraries("${UMI_TARGET}" PUBLIC Umicom::creative_workspace)
    set(example "umicom-${UMI_NAME}-creative-workspace-example")
    add_executable("${example}" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../examples/creative_workspace_main.c")
    target_compile_definitions("${example}" PRIVATE UMICOM_CREATIVE_PROFILE="${UMI_NAME}")
    target_link_libraries("${example}" PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target("${example}")
    install(TARGETS "${example}" RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
    if(BUILD_TESTING)
        add_test(NAME "${UMI_NAME}.creative_workspace.example" COMMAND "${example}")
        set_tests_properties("${UMI_NAME}.creative_workspace.example" PROPERTIES
            TIMEOUT 30 LABELS "creative-workspace;example")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${example}")
        endif()
    endif()
endfunction()
endif()
install(FILES "${_umicom_creative_root}/docs/learning/CREATIVE_WORKSPACE.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)
unset(_umicom_creative_root)

# The native clip tools reuse this service's owned export and file contracts.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomCreativeAudio.cmake")

# The offline audio tutorial keeps the original core scene and PCM services.
# Full Framework continues to register every product extension and its tests.
if(NOT _umicom_creative_audio_tutorial)
    target_sources(umicom_creative_workspace PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/export_writer_json.c")
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomCreativeAssets.cmake")

    # Song planning shares the creative asset writer and has no provider dependency.
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomSongPlan.cmake")

    # Timed audio assemblies reuse the library and the existing PCM decoder.
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAudioArrangement.cmake")

    # Caption ownership and timing are independent of the selected creative product.
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomCaptions.cmake")

    # Local video editing and rendering reuse the creative owner and native process channel.
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomVideoSequence.cmake")

    # Explicit source excerpts compose the existing generators and provider chat.
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomMediaBrief.cmake")
endif()
