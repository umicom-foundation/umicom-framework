/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/types.h
 *
 * PURPOSE:
 *   Define stable shared vocabulary for persistent terminal, supervised process and remote-development operations.
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
#ifndef UMICOM_TERMINAL_REMOTE_TYPES_H
#define UMICOM_TERMINAL_REMOTE_TYPES_H
#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_TERMINAL_REMOTE_ID_CAPACITY 128U
#define UMI_TERMINAL_REMOTE_TEXT_CAPACITY 256U
#define UMI_TERMINAL_REMOTE_PATH_CAPACITY 1024U
#define UMI_TERMINAL_REMOTE_CATALOGUE_CAPACITY 32U

/**
 * List the named terminal remote state values accepted by this public contract.
 */
typedef enum UmiTerminalRemoteState {
    UMI_TERMINAL_REMOTE_STATE_UNKNOWN = 0,
    UMI_TERMINAL_REMOTE_STATE_READY = 1,
    UMI_TERMINAL_REMOTE_STATE_ACTIVE = 2,
    UMI_TERMINAL_REMOTE_STATE_DEGRADED = 3,
    UMI_TERMINAL_REMOTE_STATE_RECONNECTING = 4,
    UMI_TERMINAL_REMOTE_STATE_STOPPED = 5,
    UMI_TERMINAL_REMOTE_STATE_FAILED = 6
} UmiTerminalRemoteState;

/**
 * List the named terminal remote health values accepted by this public contract.
 */
typedef enum UmiTerminalRemoteHealth {
    UMI_TERMINAL_REMOTE_HEALTH_UNKNOWN = 0,
    UMI_TERMINAL_REMOTE_HEALTH_HEALTHY = 1,
    UMI_TERMINAL_REMOTE_HEALTH_WARNING = 2,
    UMI_TERMINAL_REMOTE_HEALTH_CRITICAL = 3
} UmiTerminalRemoteHealth;

/**
 * Represent the terminal remote named entry data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteNamedEntry {
    char id[UMI_TERMINAL_REMOTE_ID_CAPACITY];
    char label[UMI_TERMINAL_REMOTE_TEXT_CAPACITY];
    uint64_t revision;
    bool enabled;
} UmiTerminalRemoteNamedEntry;

/**
 * Provide the terminal remote copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_remote_copy_text(char *destination, size_t capacity, const char *source);
/**
 * Initialise terminal remote named entry from caller-provided values so later operations
 * receive a known state.
 */
void umi_terminal_remote_named_entry_init(UmiTerminalRemoteNamedEntry *entry, const char *id, const char *label);
/**
 * Check that terminal remote named entry satisfies its contract before another service
 * relies on it.
 */
bool umi_terminal_remote_named_entry_valid(const UmiTerminalRemoteNamedEntry *entry);
/**
 * Provide the terminal remote fingerprint text operation used by this module and its
 * client applications.
 */
uint64_t umi_terminal_remote_fingerprint_text(const char *text);
/**
 * Provide the terminal remote state text operation used by this module and its client
 * applications.
 */
const char *umi_terminal_remote_state_text(UmiTerminalRemoteState state);
/**
 * Provide the terminal remote health text operation used by this module and its client
 * applications.
 */
const char *umi_terminal_remote_health_text(UmiTerminalRemoteHealth health);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_terminal_remote_named_entry_archive_encode(const UmiTerminalRemoteNamedEntry *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_terminal_remote_named_entry_archive_decode(const void *bytes, size_t byte_count,
    UmiTerminalRemoteNamedEntry *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTerminalRemoteNamedEntry object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_terminal_remote_named_entry_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTerminalRemoteNamedEntry *workspace, UmiTerminalRemoteNamedEntry *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_terminal_remote_named_entry_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTerminalRemoteNamedEntry *expected, UmiTerminalRemoteNamedEntry *value);


#ifdef __cplusplus
}
#endif
#endif
