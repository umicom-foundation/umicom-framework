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

typedef enum UmiTerminalRemoteState {
    UMI_TERMINAL_REMOTE_STATE_UNKNOWN = 0,
    UMI_TERMINAL_REMOTE_STATE_READY = 1,
    UMI_TERMINAL_REMOTE_STATE_ACTIVE = 2,
    UMI_TERMINAL_REMOTE_STATE_DEGRADED = 3,
    UMI_TERMINAL_REMOTE_STATE_RECONNECTING = 4,
    UMI_TERMINAL_REMOTE_STATE_STOPPED = 5,
    UMI_TERMINAL_REMOTE_STATE_FAILED = 6
} UmiTerminalRemoteState;

typedef enum UmiTerminalRemoteHealth {
    UMI_TERMINAL_REMOTE_HEALTH_UNKNOWN = 0,
    UMI_TERMINAL_REMOTE_HEALTH_HEALTHY = 1,
    UMI_TERMINAL_REMOTE_HEALTH_WARNING = 2,
    UMI_TERMINAL_REMOTE_HEALTH_CRITICAL = 3
} UmiTerminalRemoteHealth;

typedef struct UmiTerminalRemoteNamedEntry {
    char id[UMI_TERMINAL_REMOTE_ID_CAPACITY];
    char label[UMI_TERMINAL_REMOTE_TEXT_CAPACITY];
    uint64_t revision;
    bool enabled;
} UmiTerminalRemoteNamedEntry;

UmiStatus umi_terminal_remote_copy_text(char *destination, size_t capacity, const char *source);
void umi_terminal_remote_named_entry_init(UmiTerminalRemoteNamedEntry *entry, const char *id, const char *label);
bool umi_terminal_remote_named_entry_valid(const UmiTerminalRemoteNamedEntry *entry);
uint64_t umi_terminal_remote_fingerprint_text(const char *text);
const char *umi_terminal_remote_state_text(UmiTerminalRemoteState state);
const char *umi_terminal_remote_health_text(UmiTerminalRemoteHealth health);
#ifdef __cplusplus
}
#endif
#endif
