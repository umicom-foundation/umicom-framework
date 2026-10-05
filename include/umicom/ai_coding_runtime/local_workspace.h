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

/* Capture an absolute root once when an adapter is initialized. Relative
 * roots use the current directory at that moment, not during later edits.
 * This resolves spelling only; it creates no files and grants no approval.
 * Failure leaves outRoot unchanged. */
UmiStatus UmiAiCodingWorkspaceRootResolve(const char *root, char *outRoot, size_t capacity);

/* Shared synchronous file callbacks for coding integrations. Framework owns
 * path validation, Unicode conversion and single-file publication. The root
 * must be absolute, parents must exist, and links/non-file entries are refused
 * under the policy in platform/rooted_files.h. A worker should call these APIs.
 * Read clears both outputs on failure and refuses embedded NUL bytes because
 * patch comparisons use complete text. Capacity includes the terminator.
 * None of these calls approves a patch or replaces its conflict checks. */
UmiStatus UmiAiCodingWorkspaceReadFile(const char *root, const char *relativePath,
    char *outText, size_t capacity, size_t *outLength);
UmiStatus UmiAiCodingWorkspaceWriteFile(const char *root, const char *relativePath,
    const char *text, size_t length);
UmiStatus UmiAiCodingWorkspaceRemoveFile(const char *root, const char *relativePath);
UmiStatus UmiAiCodingWorkspaceFileExists(const char *root, const char *relativePath, int *outExists);

#ifdef __cplusplus
}
#endif
#endif
