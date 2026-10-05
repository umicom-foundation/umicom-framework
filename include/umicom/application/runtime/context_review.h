/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/context_review.h
 * PURPOSE: Prepare, inspect and apply linked context edits without partial publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_RUNTIME_CONTEXT_REVIEW_H
#define UMICOM_APPLICATION_RUNTIME_CONTEXT_REVIEW_H

#include "umicom/application/runtime/workspace_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiApplicationContextChangeKind {
    UMI_APPLICATION_CONTEXT_SET = 1,
    UMI_APPLICATION_CONTEXT_REMOVE = 2
} UmiApplicationContextChangeKind;

/* Inputs are copied during preparation. A removal needs only its group name;
 * an empty string is a valid SET value and is distinct from removing a group. */
typedef struct UmiApplicationContextChange {
    UmiApplicationContextChangeKind operation;
    char group_id[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    char value[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
} UmiApplicationContextChange;

typedef struct UmiApplicationContextReview UmiApplicationContextReview;

/* This summary and each row are copies suitable for a confirmation view.
 * Context links are presentation data, never permission to execute a command. */
typedef struct UmiApplicationContextReviewSummary {
    size_t change_count;
    size_t added_count;
    size_t updated_count;
    size_t removed_count;
    /* Bound UI values may have changed independently before preparation. */
    size_t ui_difference_count;
    uint64_t expected_context_revision;
    uint64_t expected_ui_revision;
    bool has_workbench;
    bool applied;
} UmiApplicationContextReviewSummary;

typedef struct UmiApplicationContextReviewRow {
    UmiApplicationContextChange change;
    bool existed;
    /* Previous application-cache value; this is not a separate UI snapshot. */
    char previous_value[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    /* Inspect these fields only when summary.has_workbench is true. They show
     * the actual typed UI value at the captured UI revision, including a value
     * whose type differs from the application's string cache. */
    bool ui_existed;
    bool cache_matches_ui;
    UmiUiContextSnapshot ui_previous;
} UmiApplicationContextReviewRow;

/* Call on the runtime's owning thread, with its borrowed experience catalogue
 * unchanged. Up to MAX_CONTEXT_BINDINGS changes may be reviewed together;
 * duplicate names are rejected. Removals free capacity before additions.
 * Preparation leaves runtime, workbench and output pointer unchanged on error.
 * Keep runtime and workbench alive until apply or review destruction; do not
 * reinitialise them while reviews exist. Related host writes use the same
 * owning thread. The UI store still serialises its own typed operations. */
UmiStatus umi_application_context_review_prepare(
    UmiApplicationWorkspaceRuntime *runtime,
    const UmiApplicationContextChange *changes, size_t count,
    UmiApplicationContextReview **out_review);

UmiStatus umi_application_context_review_summary(
    const UmiApplicationContextReview *review,
    UmiApplicationContextReviewSummary *out_summary);
UmiStatus umi_application_context_review_row(
    const UmiApplicationContextReview *review, size_t index,
    UmiApplicationContextReviewRow *out_row);

/* Only the original runtime may accept a review, once, while its session,
 * context entries and bound UI revision still match. Failure changes neither
 * context store. Re-prepare after a stale review; never force it through.
 * Unrelated UI keys are preserved. A removed binding must still exist in UI.
 * If summary.ui_difference_count is nonzero, show both application and UI
 * values before acceptance: applying explicitly replaces the affected UI keys.
 * This preserves the setter's existing ability to update an independently
 * changed UI value while making that difference available for review.
 * Applying does not execute commands, orders, payments or external requests. */
UmiStatus umi_application_context_review_apply(
    UmiApplicationWorkspaceRuntime *runtime, UmiApplicationContextReview *review);
void umi_application_context_review_destroy(UmiApplicationContextReview *review);

#ifdef __cplusplus
}
#endif
#endif
