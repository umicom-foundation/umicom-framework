/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/bindings.h
 *
 * PURPOSE:
 *   Bind the canonical Framework services participating in an IDE workspace.
 *
 * IMPORTANT:
 *   The binding owns no service. Lifetimes remain with the existing Framework
 *   composition root. This prevents duplicate source-of-truth objects.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_BINDINGS_H
#define UMICOM_IDE_INTEGRATION_BINDINGS_H

#include "umicom/ai_developer_experience/ai_developer_experience.h"
#include "umicom/debug_runtime/platform.h"
#include "umicom/developer_productivity/navigation_service.h"
#include "umicom/developer_productivity/problem_store.h"
#include "umicom/developer_productivity/source_control.h"
#include "umicom/document/coordinator.h"
#include "umicom/language/service.h"
#include "umicom/test_platform/service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiIdeIntegrationBindings {
    UmiDocumentCoordinator *documents;
    UmiDeveloperProblemStore *problems;
    UmiTestPlatformService *tests;
    UmiDeveloperSourceControl *source_control;
    UmiDebugRuntimePlatform *debug_runtime;
    UmiLanguageService *language;
    UmiDeveloperNavigationService *navigation;
    UmiAiDeveloperExperiencePlatform *ai_developer;
    uint64_t revision;
} UmiIdeIntegrationBindings;

void umi_ide_integration_bindings_init(
    UmiIdeIntegrationBindings *bindings);

UmiStatus umi_ide_integration_bind_documents(
    UmiIdeIntegrationBindings *bindings,
    UmiDocumentCoordinator *documents);

UmiStatus umi_ide_integration_bind_problems(
    UmiIdeIntegrationBindings *bindings,
    UmiDeveloperProblemStore *problems);

UmiStatus umi_ide_integration_bind_tests(
    UmiIdeIntegrationBindings *bindings,
    UmiTestPlatformService *tests);

UmiStatus umi_ide_integration_bind_source_control(
    UmiIdeIntegrationBindings *bindings,
    UmiDeveloperSourceControl *source_control);

UmiStatus umi_ide_integration_bind_debug(
    UmiIdeIntegrationBindings *bindings,
    UmiDebugRuntimePlatform *debug_runtime);

UmiStatus umi_ide_integration_bind_language(
    UmiIdeIntegrationBindings *bindings,
    UmiLanguageService *language);

UmiStatus umi_ide_integration_bind_navigation(
    UmiIdeIntegrationBindings *bindings,
    UmiDeveloperNavigationService *navigation);

UmiStatus umi_ide_integration_bind_ai_developer(
    UmiIdeIntegrationBindings *bindings,
    UmiAiDeveloperExperiencePlatform *ai_developer);

#ifdef __cplusplus
}
#endif
#endif
