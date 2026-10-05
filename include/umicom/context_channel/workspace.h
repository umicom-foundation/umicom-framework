/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/workspace.h
 *
 * PURPOSE:
 *   Define the canonical workspace context shared by applications and panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_WORKSPACE_H
#define UMICOM_CONTEXT_CHANNEL_WORKSPACE_H
#include "umicom/context_channel/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the workspace context data shared with callers of this public contract.
 */
typedef struct UmiWorkspaceContext {
    uint32_t structure_size;
    char workspace_id[UMI_CONTEXT_TEXT_CAPACITY];
    char profile_id[UMI_CONTEXT_TEXT_CAPACITY];
    char root_path[UMI_CONTEXT_PATH_CAPACITY];
    char perspective_id[UMI_CONTEXT_TEXT_CAPACITY];
    char layout_id[UMI_CONTEXT_TEXT_CAPACITY];
    bool trusted;
    uint64_t revision;
} UmiWorkspaceContext;
/**
 * Initialise workspace context from caller-provided values so later operations receive a
 * known state.
 */
void umi_workspace_context_init(UmiWorkspaceContext *context);
/**
 * Check that workspace context satisfies its contract before another service relies on it.
 */
UmiStatus umi_workspace_context_validate(const UmiWorkspaceContext *context);
/**
 * Copy workspace context into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_workspace_context_copy(UmiWorkspaceContext *destination, const UmiWorkspaceContext *source);
/**
 * Provide the workspace context set workspace id operation used by this module and its
 * client applications.
 */
UmiStatus umi_workspace_context_set_workspace_id(UmiWorkspaceContext *context, const char *value);
/**
 * Provide the workspace context set profile id operation used by this module and its
 * client applications.
 */
UmiStatus umi_workspace_context_set_profile_id(UmiWorkspaceContext *context, const char *value);
/**
 * Provide the workspace context set root path operation used by this module and its client
 * applications.
 */
UmiStatus umi_workspace_context_set_root_path(UmiWorkspaceContext *context, const char *value);
/**
 * Provide the workspace context set perspective id operation used by this module and its
 * client applications.
 */
UmiStatus umi_workspace_context_set_perspective_id(UmiWorkspaceContext *context, const char *value);
/**
 * Provide the workspace context set layout id operation used by this module and its client
 * applications.
 */
UmiStatus umi_workspace_context_set_layout_id(UmiWorkspaceContext *context, const char *value);
/**
 * Provide the workspace context set trusted operation used by this module and its client
 * applications.
 */
UmiStatus umi_workspace_context_set_trusted(UmiWorkspaceContext *context, bool value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workspace_context_archive_encode(const UmiWorkspaceContext *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workspace_context_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkspaceContext *value);

#ifdef __cplusplus
}
#endif
#endif
