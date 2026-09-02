/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/active_context.h
 *
 * PURPOSE:
 *   Publish the current cross-domain IDE context as a compact snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_ACTIVE_CONTEXT_H
#define UMICOM_IDE_INTEGRATION_ACTIVE_CONTEXT_H
#include "umicom/ide_integration/types.h"
#include "umicom/ide_integration/bindings.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ide active context data shared with callers of this public contract.
 */
typedef struct UmiIdeActiveContext {
    UmiDocumentWorkingCopySnapshot document;
    UmiDeveloperProblemStoreSnapshot problems;
    UmiTestPlatformServiceSnapshot tests;
    UmiDeveloperSourceControlSnapshot source_control;
    UmiDebugRuntimePlatformSnapshot debug;
    UmiAiDeveloperExperienceSnapshot ai;
    char workspace_root[UMI_IDE_INTEGRATION_PATH_CAPACITY];
    uint64_t revision;
    int has_document;
    int has_problems;
    int has_tests;
    int has_source_control;
    int has_debug;
    int has_ai;
} UmiIdeActiveContext;

/**
 * Provide the ide active context snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_active_context_snapshot(
    const UmiIdeIntegrationBindings *bindings,
    const char *workspace_root,
    UmiIdeActiveContext *out_context);

#ifdef __cplusplus
}
#endif
#endif
