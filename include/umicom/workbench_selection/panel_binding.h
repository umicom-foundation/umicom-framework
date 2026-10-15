/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/panel_binding.h
 *
 * PURPOSE:
 *   Define the reusable panel selection binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PANEL_BINDING_H
#define UMICOM_WORKBENCH_SELECTION_PANEL_BINDING_H

#include "umicom/workbench_selection/types.h"
#include "umicom/base/value_archive.h"
/* Text setters preserve the field and revision on invalid or oversized input,
 * permit a suffix of the same field as input, and refuse exhausted revisions.
 * Each successful edit advances once, even when the text is unchanged. */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection panel binding data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionPanelBinding {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char secondary_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char label[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionPanelBinding;

/**
 * Initialise workbench selection panel binding from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_panel_binding_init(
    UmiWorkbenchSelectionPanelBinding *record,
    const char *record_id);
/**
 * Check that workbench selection panel binding satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_panel_binding_validate(
    const UmiWorkbenchSelectionPanelBinding *record);
/**
 * Provide the workbench selection panel binding set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_panel_binding_set_source(
    UmiWorkbenchSelectionPanelBinding *record,
    const char *source_id);
/**
 * Provide the workbench selection panel binding set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_panel_binding_set_subject(
    UmiWorkbenchSelectionPanelBinding *record,
    const char *subject_id);
/**
 * Provide the workbench selection panel binding set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_panel_binding_set_secondary(
    UmiWorkbenchSelectionPanelBinding *record,
    const char *secondary_id);
/**
 * Provide the workbench selection panel binding set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_panel_binding_set_group(
    UmiWorkbenchSelectionPanelBinding *record,
    const char *group_id);
/**
 * Provide the workbench selection panel binding set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_panel_binding_set_label(
    UmiWorkbenchSelectionPanelBinding *record,
    const char *label);
/**
 * Provide the workbench selection panel binding hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_panel_binding_hash(
    const UmiWorkbenchSelectionPanelBinding *record);
/**
 * Provide the workbench selection panel binding touch operation used by this module and
 * its client applications.
 */
/** Update sequence and time only when the record revision can advance.
 * This legacy void operation preserves every field at UINT64_MAX. Use
 * replace_if_current when a caller needs an explicit refusal status. */
void umi_workbench_selection_panel_binding_touch(
    UmiWorkbenchSelectionPanelBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_selection_panel_binding_archive_encode(const UmiWorkbenchSelectionPanelBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_selection_panel_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchSelectionPanelBinding *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_workbench_selection_panel_binding_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiWorkbenchSelectionPanelBinding *expected, UmiWorkbenchSelectionPanelBinding *value);


/** Publish a reviewed panel binding value while its observed revision matches.
 * Both values must pass domain validation and retain the same record_id.
 * A stale token returns INVALID_STATE; an exhausted token returns
 * CAPACITY_EXCEEDED. Every refusal preserves the complete live value.
 * Success copies the proposal and assigns one new owner revision, ignoring
 * the proposal's token. Self-assignment is allowed; other overlap is not.
 * Serialize access on the owner. This edits an inline value only: it does
 * not publish events, activate a provider, authenticate evidence or perform I/O.
 * Constructors remain for new values and must not reset a live review token. */
UmiStatus umi_workbench_selection_panel_binding_replace_if_current(UmiWorkbenchSelectionPanelBinding *value,
    uint64_t expected_revision, const UmiWorkbenchSelectionPanelBinding *proposal);

#ifdef __cplusplus
}
#endif
#endif
