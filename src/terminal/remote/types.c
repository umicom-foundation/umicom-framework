/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/types.c
 *
 * PURPOSE:
 *   Implement common bounded strings, fingerprints and state text for remote development.
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
#include "umicom/terminal/remote/types.h"
#include <string.h>
UmiStatus umi_terminal_remote_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}
void umi_terminal_remote_named_entry_init(UmiTerminalRemoteNamedEntry *entry, const char *id, const char *label)
{
    if (entry == NULL) return;
    (void)memset(entry, 0, sizeof(*entry));
    if (id != NULL) (void)umi_terminal_remote_copy_text(entry->id, sizeof(entry->id), id);
    if (label != NULL) (void)umi_terminal_remote_copy_text(entry->label, sizeof(entry->label), label);
    entry->enabled = true;
    entry->revision = 1U;
}
bool umi_terminal_remote_named_entry_valid(const UmiTerminalRemoteNamedEntry *entry)
{
    return entry != NULL && entry->id[0] != '\0' && entry->revision > 0U;
}
uint64_t umi_terminal_remote_fingerprint_text(const char *text)
{
    uint64_t value = UINT64_C(1469598103934665603);
    const unsigned char *cursor = (const unsigned char *)(text != NULL ? text : "");
    while (*cursor != 0U) { value ^= (uint64_t)*cursor++; value *= UINT64_C(1099511628211); }
    return value;
}
const char *umi_terminal_remote_state_text(UmiTerminalRemoteState state)
{
    switch (state) {
        case UMI_TERMINAL_REMOTE_STATE_READY: return "ready";
        case UMI_TERMINAL_REMOTE_STATE_ACTIVE: return "active";
        case UMI_TERMINAL_REMOTE_STATE_DEGRADED: return "degraded";
        case UMI_TERMINAL_REMOTE_STATE_RECONNECTING: return "reconnecting";
        case UMI_TERMINAL_REMOTE_STATE_STOPPED: return "stopped";
        case UMI_TERMINAL_REMOTE_STATE_FAILED: return "failed";
        default: return "unknown";
    }
}
const char *umi_terminal_remote_health_text(UmiTerminalRemoteHealth health)
{
    switch (health) {
        case UMI_TERMINAL_REMOTE_HEALTH_HEALTHY: return "healthy";
        case UMI_TERMINAL_REMOTE_HEALTH_WARNING: return "warning";
        case UMI_TERMINAL_REMOTE_HEALTH_CRITICAL: return "critical";
        default: return "unknown";
    }
}
