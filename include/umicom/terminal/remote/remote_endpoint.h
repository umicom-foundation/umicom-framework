/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_endpoint.h
 *
 * PURPOSE:
 *   Represent a remote-development endpoint with host/port and transport identity.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_ENDPOINT_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_ENDPOINT_H
#include "umicom/terminal/remote/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote remote endpoint data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteRemoteEndpoint { char host[UMI_TERMINAL_REMOTE_TEXT_CAPACITY]; uint16_t port; bool secure; } UmiTerminalRemoteRemoteEndpoint;
/**
 * Initialise terminal remote remote endpoint from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_endpoint_init(UmiTerminalRemoteRemoteEndpoint *value,const char *host,uint16_t port,bool secure);
/**
 * Check that terminal remote remote endpoint satisfies its contract before another service
 * relies on it.
 */
bool umi_terminal_remote_remote_endpoint_valid(const UmiTerminalRemoteRemoteEndpoint *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_terminal_remote_remote_endpoint_archive_encode(const UmiTerminalRemoteRemoteEndpoint *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_terminal_remote_remote_endpoint_archive_decode(const void *bytes, size_t byte_count,
    UmiTerminalRemoteRemoteEndpoint *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTerminalRemoteRemoteEndpoint object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_terminal_remote_remote_endpoint_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTerminalRemoteRemoteEndpoint *workspace, UmiTerminalRemoteRemoteEndpoint *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_terminal_remote_remote_endpoint_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTerminalRemoteRemoteEndpoint *expected, UmiTerminalRemoteRemoteEndpoint *value);


#ifdef __cplusplus
}
#endif
#endif
