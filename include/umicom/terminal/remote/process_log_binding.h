/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/process_log_binding.h
 *
 * PURPOSE:
 *   Define validated process log binding relationships between Framework-owned resources.
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
#ifndef UMICOM_TERMINAL_REMOTE_PROCESS_LOG_BINDING_H
#define UMICOM_TERMINAL_REMOTE_PROCESS_LOG_BINDING_H
#include "umicom/terminal/remote/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote process log binding data shared with callers of this
 * public contract.
 */
typedef struct UmiTerminalRemoteProcessLogBinding { char left_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; char right_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; uint64_t revision; bool enabled; } UmiTerminalRemoteProcessLogBinding;
/**
 * Initialise terminal remote process log binding from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_process_log_binding_init(UmiTerminalRemoteProcessLogBinding *value,const char *left_id,const char *right_id);
/**
 * Check that terminal remote process log binding satisfies its contract before another
 * service relies on it.
 */
bool umi_terminal_remote_process_log_binding_valid(const UmiTerminalRemoteProcessLogBinding *value);
/**
 * Provide the terminal remote process log binding fingerprint operation used by this
 * module and its client applications.
 */
uint64_t umi_terminal_remote_process_log_binding_fingerprint(const UmiTerminalRemoteProcessLogBinding *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_terminal_remote_process_log_binding_archive_encode(const UmiTerminalRemoteProcessLogBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_terminal_remote_process_log_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiTerminalRemoteProcessLogBinding *value);

#ifdef __cplusplus
}
#endif
#endif
