/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/panel_state.h
 *
 * PURPOSE:
 *   Define the reusable panel state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_PANEL_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_PANEL_STATE_H
#include "umicom/workbench_context_source/types.h"
#include "umicom/base/value_archive.h"
/* Text setters preserve the field and revision on invalid or oversized input,
 * permit a suffix of the same field as input, and refuse exhausted revisions.
 * Each successful edit advances once, even when the text is unchanged. */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source panel state data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourcePanelState {
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
} UmiWorkbenchContextSourcePanelState;

/**
 * Initialise workbench context source panel state from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_panel_state_init(
    UmiWorkbenchContextSourcePanelState *record,
    const char *record_id);
/**
 * Check that workbench context source panel state satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_panel_state_validate(
    const UmiWorkbenchContextSourcePanelState *record);
/**
 * Provide the workbench context source panel state set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_panel_state_set_source(
    UmiWorkbenchContextSourcePanelState *record,
    const char *source_id);
/**
 * Provide the workbench context source panel state set panel operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_panel_state_set_panel(
    UmiWorkbenchContextSourcePanelState *record,
    const char *panel_id);
/**
 * Provide the workbench context source panel state set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_panel_state_set_subject(
    UmiWorkbenchContextSourcePanelState *record,
    const char *subject_id);
/**
 * Provide the workbench context source panel state set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_panel_state_set_group(
    UmiWorkbenchContextSourcePanelState *record,
    const char *group_id);
/**
 * Provide the workbench context source panel state set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_panel_state_set_label(
    UmiWorkbenchContextSourcePanelState *record,
    const char *label);
/**
 * Provide the workbench context source panel state hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_source_panel_state_hash(
    const UmiWorkbenchContextSourcePanelState *record);
/**
 * Provide the workbench context source panel state touch operation used by this module and
 * its client applications.
 */
/** Update sequence and time only when the record revision can advance.
 * This legacy void operation preserves every field at UINT64_MAX. Use
 * replace_if_current when a caller needs an explicit refusal status. */
void umi_workbench_context_source_panel_state_touch(
    UmiWorkbenchContextSourcePanelState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_source_panel_state_archive_encode(const UmiWorkbenchContextSourcePanelState *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_source_panel_state_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextSourcePanelState *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_workbench_context_source_panel_state_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiWorkbenchContextSourcePanelState *expected, UmiWorkbenchContextSourcePanelState *value);


/** Publish a reviewed panel state value while its observed revision matches.
 * Both values must pass domain validation and retain the same record_id.
 * A stale token returns INVALID_STATE; an exhausted token returns
 * CAPACITY_EXCEEDED. Every refusal preserves the complete live value.
 * Success copies the proposal and assigns one new owner revision, ignoring
 * the proposal's token. Self-assignment is allowed; other overlap is not.
 * Serialize access on the owner. This edits an inline value only: it does
 * not publish events, activate a provider, authenticate evidence or perform I/O.
 * Constructors remain for new values and must not reset a live review token. */
UmiStatus umi_workbench_context_source_panel_state_replace_if_current(UmiWorkbenchContextSourcePanelState *value,
    uint64_t expected_revision, const UmiWorkbenchContextSourcePanelState *proposal);

#ifdef __cplusplus
}
#endif
#endif
