/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/local_workspace.h
 *
 * PURPOSE:
 *   Provide a Framework-owned local filesystem workspace adapter confined to a
 *   configured repository root. This lets thin applications use governed AI
 *   patch apply/revert without reimplementing file callbacks.
 *
 * SECURITY:
 *   Every operation validates a normalized relative path before joining it to
 *   the root. Absolute paths and parent traversal are rejected.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_LOCAL_WORKSPACE_H
#define UMICOM_AI_CODING_RUNTIME_LOCAL_WORKSPACE_H

#include "umicom/ai_coding_runtime/workspace.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCodingLocalWorkspace UmiAiCodingLocalWorkspace;

UmiStatus umi_ai_coding_local_workspace_create(
    const char *root,
    UmiAiCodingLocalWorkspace **out_workspace);

void umi_ai_coding_local_workspace_destroy(
    UmiAiCodingLocalWorkspace *workspace);

UmiStatus umi_ai_coding_local_workspace_adapter(
    UmiAiCodingLocalWorkspace *workspace,
    UmiAiCodingWorkspaceAdapter *out_adapter);

const char *umi_ai_coding_local_workspace_root(
    const UmiAiCodingLocalWorkspace *workspace);

#ifdef __cplusplus
}
#endif
#endif
