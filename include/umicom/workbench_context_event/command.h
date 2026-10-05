/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/command.h
 *
 * PURPOSE:
 *   Define the reusable event command contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_COMMAND_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_COMMAND_H

#include "umicom/workbench_context_event/event.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event command data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextEventCommand {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_EVENT_TEXT_CAPACITY];
    UmiWorkbenchContextEventKind event_kind;
    UmiContextKind context_kind;
    UmiWorkbenchContextEventPriority priority;
    UmiWorkbenchContextEventState state;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextEventCommand;

/**
 * Initialise workbench context event command from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_command_init(
    UmiWorkbenchContextEventCommand *record,
    const char *record_id);
/**
 * Check that workbench context event command satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_event_command_validate(
    const UmiWorkbenchContextEventCommand *record);
/**
 * Provide the workbench context event command set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_command_set_source(
    UmiWorkbenchContextEventCommand *record,
    const char *source_id);
/**
 * Provide the workbench context event command set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_command_set_subject(
    UmiWorkbenchContextEventCommand *record,
    const char *subject_id);
/**
 * Provide the workbench context event command set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_command_set_group(
    UmiWorkbenchContextEventCommand *record,
    const char *group_id);
/**
 * Provide the workbench context event command set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_command_set_label(
    UmiWorkbenchContextEventCommand *record,
    const char *label);
/**
 * Provide the workbench context event command hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_event_command_hash(
    const UmiWorkbenchContextEventCommand *record);
/**
 * Provide the workbench context event command touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_event_command_touch(
    UmiWorkbenchContextEventCommand *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_event_command_archive_encode(const UmiWorkbenchContextEventCommand *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_event_command_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextEventCommand *value);

#ifdef __cplusplus
}
#endif
#endif
