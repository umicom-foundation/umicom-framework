/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_workspace_binding.h
 *
 * PURPOSE:
 *   Define validated terminal workspace binding relationships between Framework-owned resources.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_WORKSPACE_BINDING_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_WORKSPACE_BINDING_H
#include "umicom/terminal/remote/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote terminal workspace binding data shared with callers of
 * this public contract.
 */
typedef struct UmiTerminalRemoteTerminalWorkspaceBinding { char left_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; char right_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; uint64_t revision; bool enabled; } UmiTerminalRemoteTerminalWorkspaceBinding;
/**
 * Initialise terminal remote terminal workspace binding from caller-provided values so
 * later operations receive a known state.
 */
void umi_terminal_remote_terminal_workspace_binding_init(UmiTerminalRemoteTerminalWorkspaceBinding *value,const char *left_id,const char *right_id);
/**
 * Check that terminal remote terminal workspace binding satisfies its contract before
 * another service relies on it.
 */
bool umi_terminal_remote_terminal_workspace_binding_valid(const UmiTerminalRemoteTerminalWorkspaceBinding *value);
/**
 * Provide the terminal remote terminal workspace binding fingerprint operation used by
 * this module and its client applications.
 */
uint64_t umi_terminal_remote_terminal_workspace_binding_fingerprint(const UmiTerminalRemoteTerminalWorkspaceBinding *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_terminal_remote_terminal_workspace_binding_archive_encode(const UmiTerminalRemoteTerminalWorkspaceBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_terminal_remote_terminal_workspace_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiTerminalRemoteTerminalWorkspaceBinding *value);

#ifdef __cplusplus
}
#endif
#endif
