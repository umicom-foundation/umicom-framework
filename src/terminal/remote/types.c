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
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Provide the terminal remote copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_remote_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}
/*
 * Initialise terminal remote named entry from caller-provided values so later operations
 * receive a known state.
 */
void umi_terminal_remote_named_entry_init(UmiTerminalRemoteNamedEntry *entry, const char *id, const char *label)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL) return;
    (void)memset(entry, 0, sizeof(*entry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_terminal_remote_copy_text(entry->id, sizeof(entry->id), id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (label != NULL) (void)umi_terminal_remote_copy_text(entry->label, sizeof(entry->label), label);
    entry->enabled = true;
    entry->revision = 1U;
}
/*
 * Check that terminal remote named entry satisfies its contract before another service
 * relies on it.
 */
bool umi_terminal_remote_named_entry_valid(const UmiTerminalRemoteNamedEntry *entry)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (entry == NULL) return 0;
    if (memchr(entry->id, '\0', sizeof(entry->id)) == NULL) return 0;
    if (memchr(entry->label, '\0', sizeof(entry->label)) == NULL) return 0;

    return entry != NULL && entry->id[0] != '\0' && entry->revision > 0U;
}
/*
 * Provide the terminal remote fingerprint text operation used by this module and its
 * client applications.
 */
uint64_t umi_terminal_remote_fingerprint_text(const char *text)
{
    uint64_t value = UINT64_C(1469598103934665603);
    const unsigned char *cursor = (const unsigned char *)(text != NULL ? text : "");
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) { value ^= (uint64_t)*cursor++; value *= UINT64_C(1099511628211); }
    return value;
}
/*
 * Provide the terminal remote state text operation used by this module and its client
 * applications.
 */
const char *umi_terminal_remote_state_text(UmiTerminalRemoteState state)
{
    /* Select the behaviour associated with the requested command or state value. */
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
/*
 * Provide the terminal remote health text operation used by this module and its client
 * applications.
 */
const char *umi_terminal_remote_health_text(UmiTerminalRemoteHealth health)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (health) {
        case UMI_TERMINAL_REMOTE_HEALTH_HEALTHY: return "healthy";
        case UMI_TERMINAL_REMOTE_HEALTH_WARNING: return "warning";
        case UMI_TERMINAL_REMOTE_HEALTH_CRITICAL: return "critical";
        default: return "unknown";
    }
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTerminalRemoteNamedEntryArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x329ef7f1cfd0369f);
    schema = (schema ^ (uint64_t)sizeof(((UmiTerminalRemoteNamedEntry *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTerminalRemoteNamedEntry *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTerminalRemoteNamedEntryArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTerminalRemoteNamedEntry *)0)->id) - 1U +
        8U + sizeof(((UmiTerminalRemoteNamedEntry *)0)->label) - 1U +
        8U +
        8U;
}
static void UmiTerminalRemoteNamedEntryArchiveWrite(UmiArchiveWriter *writer, const UmiTerminalRemoteNamedEntry *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiTerminalRemoteNamedEntryArchiveRead(UmiArchiveReader *reader, UmiTerminalRemoteNamedEntry *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiTerminalRemoteNamedEntryArchiveValidate(const UmiTerminalRemoteNamedEntry *value)
{
    return umi_terminal_remote_named_entry_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_terminal_remote_named_entry_archive_encode, umi_terminal_remote_named_entry_archive_decode,
    UmiTerminalRemoteNamedEntry, UmiTerminalRemoteNamedEntryArchiveSchema, UmiTerminalRemoteNamedEntryArchiveBound, UmiTerminalRemoteNamedEntryArchiveWrite, UmiTerminalRemoteNamedEntryArchiveRead, UmiTerminalRemoteNamedEntryArchiveValidate)
