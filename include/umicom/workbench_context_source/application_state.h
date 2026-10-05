/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/application_state.h
 *
 * PURPOSE:
 *   Define the reusable application state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_APPLICATION_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_APPLICATION_STATE_H
#include "umicom/workbench_context_source/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source application state data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchContextSourceApplicationState {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceApplicationState;

/**
 * Initialise workbench context source application state from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_source_application_state_init(
    UmiWorkbenchContextSourceApplicationState *record,
    const char *record_id);
/**
 * Check that workbench context source application state satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_source_application_state_validate(
    const UmiWorkbenchContextSourceApplicationState *record);
/**
 * Provide the workbench context source application state set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_application_state_set_source(
    UmiWorkbenchContextSourceApplicationState *record,
    const char *source_id);
/**
 * Provide the workbench context source application state set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_application_state_set_panel(
    UmiWorkbenchContextSourceApplicationState *record,
    const char *panel_id);
/**
 * Provide the workbench context source application state set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_source_application_state_set_subject(
    UmiWorkbenchContextSourceApplicationState *record,
    const char *subject_id);
/**
 * Provide the workbench context source application state set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_application_state_set_group(
    UmiWorkbenchContextSourceApplicationState *record,
    const char *group_id);
/**
 * Provide the workbench context source application state set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_application_state_set_label(
    UmiWorkbenchContextSourceApplicationState *record,
    const char *label);
/**
 * Provide the workbench context source application state hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_context_source_application_state_hash(
    const UmiWorkbenchContextSourceApplicationState *record);
/**
 * Provide the workbench context source application state touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_source_application_state_touch(
    UmiWorkbenchContextSourceApplicationState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_source_application_state_archive_encode(const UmiWorkbenchContextSourceApplicationState *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_source_application_state_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextSourceApplicationState *value);

#ifdef __cplusplus
}
#endif
#endif
