/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/bindings.c
 *
 * PURPOSE:
 *   Implement non-owning canonical service bindings for IDE composition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/bindings.h"

#include <string.h>

/*
 * Initialise ide integration bindings from caller-provided values so later operations
 * receive a known state.
 */
void umi_ide_integration_bindings_init(
    UmiIdeIntegrationBindings *bindings)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL) return;
    (void)memset(bindings, 0, sizeof(*bindings));
    bindings->revision = 1U;
}

#define BIND_FN(name, field, type) \
UmiStatus name(UmiIdeIntegrationBindings *bindings, type *value) \
{ \
    if (bindings == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    bindings->field = value; \
    bindings->revision += 1U; \
    return UMI_STATUS_OK; \
}

BIND_FN(
    umi_ide_integration_bind_documents,
    documents,
    UmiDocumentCoordinator)
BIND_FN(
    umi_ide_integration_bind_problems,
    problems,
    UmiDeveloperProblemStore)
BIND_FN(
    umi_ide_integration_bind_tests,
    tests,
    UmiTestPlatformService)
BIND_FN(
    umi_ide_integration_bind_source_control,
    source_control,
    UmiDeveloperSourceControl)
BIND_FN(
    umi_ide_integration_bind_debug,
    debug_runtime,
    UmiDebugRuntimePlatform)
BIND_FN(
    umi_ide_integration_bind_language,
    language,
    UmiLanguageService)
BIND_FN(
    umi_ide_integration_bind_navigation,
    navigation,
    UmiDeveloperNavigationService)
BIND_FN(
    umi_ide_integration_bind_ai_developer,
    ai_developer,
    UmiAiDeveloperExperiencePlatform)

#undef BIND_FN
