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

/**
 * Represent the ai coding local workspace data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingLocalWorkspace UmiAiCodingLocalWorkspace;

/**
 * Initialise ai coding local workspace from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_coding_local_workspace_create(
    const char *root,
    UmiAiCodingLocalWorkspace **out_workspace);

/**
 * Release or reset state held by ai coding local workspace so the same storage can be
 * reused safely.
 */
void umi_ai_coding_local_workspace_destroy(
    UmiAiCodingLocalWorkspace *workspace);

/**
 * Provide the ai coding local workspace adapter operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_local_workspace_adapter(
    UmiAiCodingLocalWorkspace *workspace,
    UmiAiCodingWorkspaceAdapter *out_adapter);

/**
 * Provide the ai coding local workspace root operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_local_workspace_root(
    const UmiAiCodingLocalWorkspace *workspace);

#ifdef __cplusplus
}
#endif
#endif
