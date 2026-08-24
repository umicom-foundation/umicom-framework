#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomApplicationRuntimeIntegration.cmake
#
# PURPOSE:
#   Add the Framework-owned application runtime, launcher, taskbar and Umicom
#   Desk runtime sources to the established Framework targets without creating
#   a competing build catalogue or duplicating existing target definitions.
#
# USAGE:
#   The Framework root includes this file after the application, desktop and
#   optional GTK4 targets have been declared. The global include guard keeps
#   transitional product-root includes harmless while they are removed.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_APPLICATION_RUNTIME_FRAMEWORK_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/..")

if(NOT TARGET umicom_application OR NOT TARGET umicom_desktop)
    message(FATAL_ERROR
        "UmicomApplicationRuntimeIntegration.cmake must be included after "
        "the Framework application and desktop targets are created.")
endif()

target_sources(umicom_application PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/launcher.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/feature_pack.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/preset.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/composition.c"
)

target_sources(umicom_desktop PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/desktop/application_strip.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/desktop/desk_runtime.c"
)

if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/desk_gtk4.c"
    )
endif()

if(BUILD_TESTING)
    function(umicom_add_application_runtime_test target test_name source)
        if(TARGET "${target}")
            return()
        endif()
        add_executable(
            "${target}"
            "${UMICOM_APPLICATION_RUNTIME_FRAMEWORK_ROOT}/${source}"
        )

        target_link_libraries("${target}" PRIVATE Umicom::Framework)

        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()

        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()

        add_test(NAME "${test_name}" COMMAND "${target}")
    endfunction()

    umicom_add_application_runtime_test(
        umicom-application-runtime-catalogue-test
        framework.application.runtime_catalogue
        tests/application/test_runtime_catalogue.c
    )
    umicom_add_application_runtime_test(
        umicom-application-launcher-test
        framework.application.launcher
        tests/application/test_launcher.c
    )
    umicom_add_application_runtime_test(
        umicom-application-feature-pack-test
        framework.application.feature_packs
        tests/application/test_feature_pack.c
    )
    umicom_add_application_runtime_test(
        umicom-application-preset-test
        framework.application.presets
        tests/application/test_preset.c
    )
    umicom_add_application_runtime_test(
        umicom-application-composition-test
        framework.application.composition
        tests/application/test_composition.c
    )
    umicom_add_application_runtime_test(
        umicom-desktop-application-strip-test
        framework.desktop.application_strip
        tests/desktop/test_application_strip.c
    )
    umicom_add_application_runtime_test(
        umicom-desk-runtime-test
        framework.desktop.desk_runtime
        tests/desktop/test_desk_runtime.c
    )
endif()

message(STATUS
    "Umicom application launcher, thin-application composition and Desk runtime enabled")

# Persistent layout documents, transactions, history, recovery and portable
# .umilayout resources are part of Framework rather than a product repository.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomWorkbenchLayoutPlatform.cmake")

# Typed cross-application context routing and panel hosting are Framework
# capabilities shared by Desk, Studio, Trader, TMS and future products.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomContextChannelPlatform.cmake")

# Reusable IDE/workstation shell contributions, command routing and drag/drop.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationShellPlatform.cmake")

# Professional developer-workbench command, search, project and perspective
# composition is layered on the existing developer/application services.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomDeveloperWorkbenchPlatform.cmake")

# Framework-owned project models, language packs and starter generation extend
# the professional developer workbench without moving project logic into Studio.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomDeveloperProjectPlatform.cmake")

# Professional Source Control, compare/merge, terminal, Problems/output and
# navigation workflows extend the existing project/workbench platform.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomDeveloperProductivityPlatform.cmake")

# Persistent Language Server Protocol / IntelliSense runtime composes the
# existing provider-neutral Language Service and Editor Session contracts.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomLanguageRuntimePlatform.cmake")

# Real persistent Debug Adapter Protocol runtime composes the existing Debug
# Service, advanced inspection models and stable adapter contract.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomDebugRuntimePlatform.cmake")

# Operational AI coding/vibe-coding runtime composes AuthorEngine, the existing
# coding assistant, developer execution and governed workspace transactions.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiCodingRuntimePlatform.cmake")

# Controlled AI developer tools, checkpoints, multi-step plans and repository-
# aware tool-calling chat build on the operational AI Coding Runtime.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiCodingToolsPlatform.cmake")

# Toolkit-neutral AI Developer Experience views, approvals, durable sessions,
# diff/review workflows and command composition build on the operational coding
# runtime and controlled developer tools.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiDeveloperExperiencePlatform.cmake")

# Cross-domain IDE integration composes canonical document/editor, Problems,
# Test Platform, Source Control, Debug, language intelligence, AI Developer,
# workflow/self-host gates and toolkit-neutral IDE surfaces.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomIdeIntegrationPlatform.cmake")

# Framework-owned Umicom Studio runtime composition: shell/IDE command
# activation, editor/document synchronization, semantic layouts, status, command
# search, runtime closure and thin toolkit-host boundary.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomStudioRuntimePlatform.cmake")

# Native repository control plane, dependency topology, lock planning and
# Framework-owned CLI routing. This is additive and does not replace VCS,
# Source Control, repository scaffolding or existing application runtimes.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomRepositoryControlPlatform.cmake")

