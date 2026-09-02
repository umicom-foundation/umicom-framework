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
#ifdef __cplusplus
}
#endif
#endif
