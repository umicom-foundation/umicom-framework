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

# Framework-owned global Desk operations extend existing desktop/layout/context
# models with application, search, panel, window and session orchestration.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomDeskOperationsPlatform.cmake")

# Typed design tokens, semantic component specifications, interaction states and
# cross-application visual contracts extend canonical UI before renderer layers;
# GTK4, Qt6, Native Web and thin applications consume the same design semantics.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomUniversalDesignSystemPlatform.cmake")

# Searchable panel discovery, governed layout editing, monitor-aware restore and
# cross-application Mosaic workspaces extend the canonical workbench model;
# renderers and thin products consume the same Framework-owned layout semantics.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomWorkbenchMosaicPlatform.cmake")

# Virtualised enterprise grids, hierarchical trees, property inspectors and
# high-density selection/edit/query models extend canonical UI; Studio, Trader,
# TMS, Bank and other applications consume one Framework-owned data-view layer.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomEnterpriseDataViewPlatform.cmake")

# Declarative bindings, validators, computed expressions and dependency-aware
# reactive state propagation extend canonical UI; renderer adapters and thin
# applications consume one Framework-owned state graph and transaction model.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomReactiveUiStatePlatform.cmake")

# Production visual application/form/page authoring, canvas editing, bindings,
# preview and inspectable declarative generation extend canonical designer;
# Studio remains a thin frontend over the Framework-owned RAD designer engine.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomVisualApplicationDesignerPlatform.cmake")

# Adaptive device families, safe areas, navigation/shell adaptation and responsive
# preview matrices extend canonical UI/designer semantics; applications remain
# thin compositions over one Framework-owned multi-device application platform.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomAdaptiveMultiDevicePlatform.cmake")

# Universal workstation surfaces, reusable layout presets and GTK4 renderers
# extend the canonical UI workbench for Studio, Trader, media applications,
# Umicom OS, Kitchen Designer and future thin application compositions.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomGtk4WorkstationPlatform.cmake")

# Qt6 is an optional frontend renderer for the same canonical workstation,
# component, layout and context contracts. Qt/C++ remains isolated here while
# Framework semantics and application ownership stay toolkit-neutral and C23.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomQt6WorkstationPlatform.cmake")

# Native server-driven browser rendering extends the existing Umicom::web and
# Umicom::frontend targets with C23 HTML/CSS/DOM projection. It deliberately
# requires neither Wt nor WebAssembly and never makes GTK4 own browser state.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomNativeWebFrontendPlatform.cmake")

# Semantic frontend conformance, golden snapshots, responsive adaptation and
# release gates verify GTK4, Qt6, Native Web and future renderers against the
# same Framework-owned workstation/component contracts.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomFrontendConformancePlatform.cmake")

# Cross-platform runtime profiles, application bundles, package/installer plans,
# update channels, rollback and deployment gates extend canonical distribution
# and delivery services while applications remain thin Framework consumers.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomCrossPlatformDistributionPlatform.cmake")

# Portable schema metadata, ORM mapping, query planning, pooled connections,
# migration graphs, replication/read routing, backup/restore and Data Server
# operations extend canonical umicom_data; backend ownership remains unchanged.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomDataServerEnterprisePlatform.cmake")

# Enterprise Integration Fabric service discovery, schema governance, routing,
# connectors, workflows, sagas and distributed-service health extend canonical
# umicom_integration and reuse existing messaging/resilience/web capabilities.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomIntegrationFabricEnterprisePlatform.cmake")

# Framework-owned dependency injection, scoped service graphs, conditional
# auto-configuration and reusable starters extend canonical runtime services;
# application repositories remain thin composition and identity layers.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomRuntimeBootstrapPlatform.cmake")

# Cross-target portability, RISC-V runtime profiles and Umicom OS boot/service
# contracts extend canonical platform services while existing compiler/toolchain
# discovery remains authoritative and applications remain thin compositions.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomCrossTargetPlatform.cmake")

# Native C23 compiler front-end foundations, compact IR, optimization, target
# lowering, object/link planning, incremental compilation and Umicc self-host
# gates extend canonical umicom_compiler while external providers remain usable.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomNativeCompilerPlatform.cmake")

# Governed autonomous engineering, repair/regeneration feedback loops, validation,
# checkpoints, rollback and bounded software evolution extend canonical developer
# services while existing compiler/VCS/test/repository engines remain authoritative.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomHelixAutonomousEngineeringPlatform.cmake")

# Shared currencies, calendars, legal entities, books, products, trades,
# lifecycle events, cashflows, positions, settlement and valuation contracts
# extend canonical finance so all financial applications consume one core.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSharedFinancialCorePlatform.cmake")

# Treasury cash/liquidity, funding, risk, settlement, collateral, margin, custody
# and securities-finance services extend canonical finance for TMS and other
# thin financial applications without duplicating domain engines in products.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomTreasuryRiskSettlementCollateralPlatform.cmake")

# Core banking customers/accounts/lending, governed payment lifecycles/rails and
# double-entry accounting/ledger controls extend canonical finance; Bank, TMS,
# Payments and future applications remain thin Framework compositions.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomBankingPaymentsAccountingPlatform.cmake")

# Advanced order lifecycle, brokerage economics, smart routing, exchange matching,
# auctions, consolidated markets and surveillance extend canonical trading;
# Trader and Exchange remain thin Framework compositions.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomTradingBrokerageExchangePlatform.cmake")

# Physical commodity inventory/logistics/delivery and provider-neutral digital
# asset network/custody/transfer services extend canonical finance while generic
# execution and exchange semantics remain owned by canonical trading.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomCommodityDigitalAssetPlatform.cmake")

# Advanced products, term structures, option models, sensitivities, scenarios and
# XVA extend canonical finance; TMS, Trader, Bank and future applications remain
# thin consumers of one Framework-owned quantitative pricing platform.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomAdvancedFinancialQuantPlatform.cmake")

# Enterprise market-data cuts, calibration workflows, distributed valuation grids,
# portfolio aggregation, VaR/ES, scenario/stress and P&L explain services extend
# canonical finance while financial applications remain thin Framework consumers.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomEnterpriseValuationRiskPlatform.cmake")

# Regulatory taxonomies/submissions, lineage, data-quality, reconciliation,
# capital/liquidity measures and financial controls extend canonical finance;
# regulated applications remain thin consumers of one governed Framework core.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomRegulatoryReportingGovernancePlatform.cmake")

# Prudential RWA, capital buffers/headroom, liquidity ladders/LCR/NSFR and
# governed enterprise stress programmes extend canonical finance; TMS, Bank,
# Risk and regulated applications remain thin Framework consumers.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomPrudentialCapitalLiquidityStressPlatform.cmake")

# Reusable IDE/workstation shell contributions, command routing and drag/drop.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationShellPlatform.cmake")

# Professional developer-workbench command, search, project and perspective
# composition is layered on the existing developer/application services.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomDeveloperWorkbenchPlatform.cmake")

# Framework-owned project models, language packs and starter generation extend
# the professional developer workbench without moving project logic into Studio.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomDeveloperProjectPlatform.cmake")

# Multi-root workspace/project orchestration, project graph, CMake understanding,
# reusable profiles, import/migration and New Umicom Application generation all
# remain Framework-owned and are consumed by Studio/Desk as thin frontends.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomProjectWorkspacePlatform.cmake")

# Professional Source Control, compare/merge, terminal, Problems/output and
# navigation workflows extend the existing project/workbench platform.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomDeveloperProductivityPlatform.cmake")

# Advanced Git history, blame, partial staging, branches/tags/stash, safe history
# operations, conflict resolution and Beyond Compare-style diff/merge services
# extend the canonical VCS target. Frontends remain thin Framework consumers.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomVcsAdvancedPlatform.cmake")

# Persistent terminal sessions, supervised process/service state, remote
# workspaces, remote command/file operations, port forwarding and remote
# language/debug bridges extend the canonical terminal target.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomTerminalRemoteDevelopmentPlatform.cmake")

# Persistent Language Server Protocol / IntelliSense runtime composes the
# existing provider-neutral Language Service and Editor Session contracts.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomLanguageRuntimePlatform.cmake")

# Native language-intelligence orchestration extends the established language
# target with provider selection, routing, request governance, workspace indexes,
# semantic state and advanced editor intelligence. Applications remain thin.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomLanguageIntelligencePlatform.cmake")

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

# Framework-owned model routing, context/retrieval, workspace memory, inline
# assistance, code review and governed agent policy extend existing AI runtimes.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiDeveloperPlatform.cmake")

# Framework-owned AI Teacher curriculum, exercises, compiler/test assessment,
# adaptive mastery and durable learning state compose existing developer/AI
# services while keeping every application a thin presentation layer.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiTeacherPlatform.cmake")

# Stable extension ABI negotiation, trust/permission policy, package lifecycle,
# isolated extension-host supervision, quarantine and recovery extend the
# canonical Framework plug-in subsystem; applications remain thin consumers.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomPluginExtensionHostPlatform.cmake")

# Framework-owned profiling, telemetry, benchmarks, coverage, memory analysis,
# performance budgets and toolkit-neutral operational dashboards extend the
# canonical observability target without moving reusable logic into applications.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomObservabilityPerformancePlatform.cmake")

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

# Operation-scoped tool discovery lets repository, VCS and other native commands
# require only the executables they actually use rather than the full compiler
# stack. Repository read/write commands therefore remain usable when a compiler
# probe is unavailable or intentionally omitted.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomToolchainOperationPlatform.cmake")

# Complete development kits compose existing compiler profiles, SDK discovery,
# target/sysroot state and operation-scoped tools for CLI, Desk and thin apps.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomToolchainKitPlatform.cmake")

# Toolkit-neutral repository maintenance/doctor state builds on the native
# repository control plane without replacing existing VCS or Source Control.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomRepositoryMaintenancePlatform.cmake")

# Read-only Git-backed probes populate the existing maintenance and control
# models; no parallel VCS or status model is introduced.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomRepositoryInspectionRuntimePlatform.cmake")

# Remediation is represented as an explicit reviewable plan. This phase does not
# automatically reset, clean, checkout, commit, push, remove or rewrite history.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomRepositoryRemediationPlanningPlatform.cmake")

# Toolkit-neutral repository operations compose inspection and planning so CLI,
# Studio and Desk can consume one Framework-owned backend.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomRepositoryOperationsPlatform.cmake")
