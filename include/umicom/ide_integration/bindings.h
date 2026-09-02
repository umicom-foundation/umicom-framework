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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the ide integration bindings data shared with callers of this public contract.
 */
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

/**
 * Initialise ide integration bindings from caller-provided values so later operations
 * receive a known state.
 */
void umi_ide_integration_bindings_init(
    UmiIdeIntegrationBindings *bindings);

/**
 * Provide the ide integration bind documents operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_integration_bind_documents(
    UmiIdeIntegrationBindings *bindings,
    UmiDocumentCoordinator *documents);

/**
 * Provide the ide integration bind problems operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_integration_bind_problems(
    UmiIdeIntegrationBindings *bindings,
    UmiDeveloperProblemStore *problems);

/**
 * Provide the ide integration bind tests operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_integration_bind_tests(
    UmiIdeIntegrationBindings *bindings,
    UmiTestPlatformService *tests);

/**
 * Provide the ide integration bind source control operation used by this module and its
 * client applications.
 */
UmiStatus umi_ide_integration_bind_source_control(
    UmiIdeIntegrationBindings *bindings,
    UmiDeveloperSourceControl *source_control);

/**
 * Provide the ide integration bind debug operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_integration_bind_debug(
    UmiIdeIntegrationBindings *bindings,
    UmiDebugRuntimePlatform *debug_runtime);

/**
 * Provide the ide integration bind language operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_integration_bind_language(
    UmiIdeIntegrationBindings *bindings,
    UmiLanguageService *language);

/**
 * Provide the ide integration bind navigation operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_integration_bind_navigation(
    UmiIdeIntegrationBindings *bindings,
    UmiDeveloperNavigationService *navigation);

/**
 * Provide the ide integration bind ai developer operation used by this module and its
 * client applications.
 */
UmiStatus umi_ide_integration_bind_ai_developer(
    UmiIdeIntegrationBindings *bindings,
    UmiAiDeveloperExperiencePlatform *ai_developer);

#ifdef __cplusplus
}
#endif
#endif
