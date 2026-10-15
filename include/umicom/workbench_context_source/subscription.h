/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/subscription.h
 *
 * PURPOSE:
 *   Define the reusable source subscription contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_SUBSCRIPTION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_SUBSCRIPTION_H
#include "umicom/workbench_context_source/types.h"
#include "umicom/base/value_archive.h"
/* Text setters preserve the field and revision on invalid or oversized input,
 * permit a suffix of the same field as input, and refuse exhausted revisions.
 * Each successful edit advances once, even when the text is unchanged. */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source subscription data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceSubscription {
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
} UmiWorkbenchContextSourceSubscription;

/**
 * Initialise workbench context source subscription from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_subscription_init(
    UmiWorkbenchContextSourceSubscription *record,
    const char *record_id);
/**
 * Check that workbench context source subscription satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_subscription_validate(
    const UmiWorkbenchContextSourceSubscription *record);
/**
 * Provide the workbench context source subscription set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_subscription_set_source(
    UmiWorkbenchContextSourceSubscription *record,
    const char *source_id);
/**
 * Provide the workbench context source subscription set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_subscription_set_panel(
    UmiWorkbenchContextSourceSubscription *record,
    const char *panel_id);
/**
 * Provide the workbench context source subscription set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_subscription_set_subject(
    UmiWorkbenchContextSourceSubscription *record,
    const char *subject_id);
/**
 * Provide the workbench context source subscription set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_subscription_set_group(
    UmiWorkbenchContextSourceSubscription *record,
    const char *group_id);
/**
 * Provide the workbench context source subscription set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_subscription_set_label(
    UmiWorkbenchContextSourceSubscription *record,
    const char *label);
/**
 * Provide the workbench context source subscription hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_source_subscription_hash(
    const UmiWorkbenchContextSourceSubscription *record);
/**
 * Provide the workbench context source subscription touch operation used by this module
 * and its client applications.
 */
/** Update sequence and time only when the record revision can advance.
 * This legacy void operation preserves every field at UINT64_MAX. Use
 * replace_if_current when a caller needs an explicit refusal status. */
void umi_workbench_context_source_subscription_touch(
    UmiWorkbenchContextSourceSubscription *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_source_subscription_archive_encode(const UmiWorkbenchContextSourceSubscription *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_source_subscription_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextSourceSubscription *value);

/** Publish a reviewed subscription value while its observed revision matches.
 * Both values must pass domain validation and retain the same record_id.
 * A stale token returns INVALID_STATE; an exhausted token returns
 * CAPACITY_EXCEEDED. Every refusal preserves the complete live value.
 * Success copies the proposal and assigns one new owner revision, ignoring
 * the proposal's token. Self-assignment is allowed; other overlap is not.
 * Serialize access on the owner. This edits an inline value only: it does
 * not publish events, activate a provider, authenticate evidence or perform I/O.
 * Constructors remain for new values and must not reset a live review token. */
UmiStatus umi_workbench_context_source_subscription_replace_if_current(UmiWorkbenchContextSourceSubscription *value,
    uint64_t expected_revision, const UmiWorkbenchContextSourceSubscription *proposal);

#ifdef __cplusplus
}
#endif
#endif
