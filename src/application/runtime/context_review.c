/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/context_review.c
 * PURPOSE: Keep reviewed context changes atomic across application and UI stores.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/runtime/context_review.h"
#include "umicom/ui/context_changes.h"
#include <stdlib.h>
#include <string.h>

/* The review owns all editable text. It borrows only stable runtime, workbench
 * and catalogue identities; callers must keep those objects alive. */
struct UmiApplicationContextReview {
    UmiApplicationWorkspaceRuntime *owner;
    UmiUiWorkbench *workbench;
    UmiApplicationSession session;
    UmiApplicationContextBindingStore expected;
    UmiApplicationContextBindingStore candidate;
    UmiApplicationContextReviewSummary summary;
    UmiApplicationContextReviewRow rows[UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS];
    UmiUiContextChange ui_changes[UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS];
};

/* Public runtime values are validated before any count controls a traversal. */
static UmiStatus context_runtime_validate(const UmiApplicationWorkspaceRuntime *runtime)
{
    UmiStatus status;
    if (runtime == NULL || runtime->structure_size != sizeof(*runtime))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_session_validate(&runtime->session);
    if (status != UMI_STATUS_OK) return status;
    return umi_application_context_binding_validate(&runtime->contexts);
}

/* Compare meaningful fields, not structure padding or unused array slots.
 * This also catches edits to the public value store that bypass its revision. */
static int context_store_equal(const UmiApplicationContextBindingStore *left,
    const UmiApplicationContextBindingStore *right)
{
    size_t index;
    if (left->revision != right->revision || left->entry_count != right->entry_count)
        return 0;
    for (index = 0U; index < left->entry_count; ++index)
        if (left->entries[index].revision != right->entries[index].revision ||
            strcmp(left->entries[index].group_id, right->entries[index].group_id) != 0 ||
            strcmp(left->entries[index].value, right->entries[index].value) != 0)
            return 0;
    return 1;
}

/* Catalogue pointers are stable for the lifetime of a session. Compare panel
 * identities too, so a direct session edit cannot reuse an obsolete review. */
static int context_session_equal(const UmiApplicationSession *left,
    const UmiApplicationSession *right)
{
    size_t index;
    if (left->revision != right->revision || left->experience != right->experience ||
        left->layout != right->layout || left->layout_locked != right->layout_locked ||
        left->active_panel_count != right->active_panel_count) return 0;
    for (index = 0U; index < left->active_panel_count; ++index)
        if (left->active_panel_ids[index] != right->active_panel_ids[index]) return 0;
    return 1;
}

UmiStatus umi_application_context_review_prepare(
    UmiApplicationWorkspaceRuntime *runtime,
    const UmiApplicationContextChange *changes, size_t count,
    UmiApplicationContextReview **out_review)
{
    UmiApplicationContextReview *review;
    UmiStatus status = context_runtime_validate(runtime);
    size_t index, previous;
    if (status != UMI_STATUS_OK) return status;
    if (out_review == NULL || (count != 0U && changes == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS ||
        (count != 0U && runtime->contexts.revision == UINT64_MAX))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Reject malformed or repeated keys before allocating or staging edits. */
    for (index = 0U; index < count; ++index) {
        if (memchr(changes[index].group_id, '\0', sizeof(changes[index].group_id)) == NULL ||
            changes[index].group_id[0] == '\0' ||
            (changes[index].operation != UMI_APPLICATION_CONTEXT_SET &&
             changes[index].operation != UMI_APPLICATION_CONTEXT_REMOVE) ||
            (changes[index].operation == UMI_APPLICATION_CONTEXT_SET &&
             memchr(changes[index].value, '\0', sizeof(changes[index].value)) == NULL))
            return UMI_STATUS_INVALID_ARGUMENT;
        for (previous = 0U; previous < index; ++previous)
            if (strcmp(changes[index].group_id, changes[previous].group_id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
    }
    review = calloc(1U, sizeof(*review));
    if (review == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    review->owner = runtime;
    review->workbench = runtime->workbench;
    review->session = runtime->session;
    review->expected = runtime->contexts;
    review->candidate = runtime->contexts;
    review->summary.change_count = count;
    review->summary.expected_context_revision = runtime->contexts.revision;
    review->summary.has_workbench = runtime->workbench != NULL;
    /* A single locked read captures actual typed UI values and their revision.
     * A confirmation view can show cache/UI differences already present before
     * preparation, rather than describing the cache as the current UI value. */
    if (runtime->workbench != NULL) {
        const char *keys[UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS] = {0};
        UmiUiContextObservation observations[UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS] = {0};
        for (index = 0U; index < count; ++index) keys[index] = changes[index].group_id;
        status = UmiUiContextReadKeys(umi_ui_workbench_context(runtime->workbench),
            keys, count, observations, &review->summary.expected_ui_revision);
        if (status != UMI_STATUS_OK) goto failed;
        for (index = 0U; index < count; ++index) {
            const char *cached = umi_application_context_binding_get(
                &review->expected, changes[index].group_id);
            UmiApplicationContextReviewRow *row = &review->rows[index];
            row->ui_existed = observations[index].found != 0;
            row->ui_previous = observations[index].value;
            row->cache_matches_ui = cached == NULL ? !row->ui_existed :
                row->ui_existed && row->ui_previous.kind == UMI_UI_CONTEXT_STRING &&
                strcmp(cached, row->ui_previous.string_value) == 0;
            if (!row->cache_matches_ui) ++review->summary.ui_difference_count;
        }
    }
    /* Intermediate staging revisions are private. The complete publication
     * advances the public store once, even when it contains several edits. */
    review->candidate.revision = 0U;
    for (index = 0U; index < count; ++index) {
        const char *old = umi_application_context_binding_get(
            &review->expected, changes[index].group_id);
        UmiApplicationContextReviewRow *row = &review->rows[index];
        /* Copy only meaningful fields. A removal's unused value may contain
         * arbitrary bytes; review consumers should receive a safe empty string. */
        row->change.operation = changes[index].operation;
        memcpy(row->change.group_id, changes[index].group_id,
            strlen(changes[index].group_id) + 1U);
        if (changes[index].operation == UMI_APPLICATION_CONTEXT_SET)
            memcpy(row->change.value, changes[index].value, strlen(changes[index].value) + 1U);
        row->existed = old != NULL;
        if (old != NULL) memcpy(row->previous_value, old, strlen(old) + 1U);
        memcpy(review->ui_changes[index].value.key, changes[index].group_id,
            strlen(changes[index].group_id) + 1U);
        if (changes[index].operation == UMI_APPLICATION_CONTEXT_REMOVE) {
            status = umi_application_context_binding_clear(&review->candidate,
                changes[index].group_id);
            if (status != UMI_STATUS_OK) goto failed;
            review->ui_changes[index].operation = UMI_UI_CONTEXT_CHANGE_REMOVE;
            ++review->summary.removed_count;
        } else {
            review->ui_changes[index].operation = UMI_UI_CONTEXT_CHANGE_SET;
            review->ui_changes[index].value.kind = UMI_UI_CONTEXT_STRING;
            memcpy(review->ui_changes[index].value.string_value, changes[index].value,
                strlen(changes[index].value) + 1U);
            if (old == NULL) ++review->summary.added_count;
            else ++review->summary.updated_count;
        }
    }
    /* Process SET only after all removals, allowing a full workspace to replace
     * a group without temporarily exceeding its fixed storage capacity. */
    for (index = 0U; index < count; ++index) {
        if (changes[index].operation != UMI_APPLICATION_CONTEXT_SET) continue;
        status = umi_application_context_binding_set(&review->candidate,
            changes[index].group_id, changes[index].value);
        if (status != UMI_STATUS_OK) goto failed;
    }
    review->candidate.revision = review->expected.revision + (count != 0U ? 1U : 0U);
    *out_review = review;
    return UMI_STATUS_OK;
failed:
    free(review);
    return status;
}

/* Readers receive copies so a confirmation view cannot mutate the transaction. */
UmiStatus umi_application_context_review_summary(
    const UmiApplicationContextReview *review, UmiApplicationContextReviewSummary *out_summary)
{
    if (review == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_summary = review->summary;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_context_review_row(const UmiApplicationContextReview *review,
    size_t index, UmiApplicationContextReviewRow *out_row)
{
    if (review == NULL || out_row == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= review->summary.change_count) return UMI_STATUS_NOT_FOUND;
    *out_row = review->rows[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_application_context_review_apply(
    UmiApplicationWorkspaceRuntime *runtime, UmiApplicationContextReview *review)
{
    UmiStatus status = context_runtime_validate(runtime);
    if (status != UMI_STATUS_OK) return status;
    if (review == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (review->owner != runtime || review->summary.applied ||
        review->workbench != runtime->workbench ||
        !context_session_equal(&review->session, &runtime->session) ||
        !context_store_equal(&review->expected, &runtime->contexts))
        return UMI_STATUS_INVALID_STATE;
    /* Publish through the canonical UI owner first. No fallible step remains
     * between that atomic operation and the application cache assignment. */
    if (runtime->workbench != NULL) {
        status = UmiUiContextApplyChanges(umi_ui_workbench_context(runtime->workbench),
            review->summary.expected_ui_revision, review->ui_changes,
            review->summary.change_count, NULL);
        if (status != UMI_STATUS_OK) return status;
    }
    runtime->contexts = review->candidate;
    review->summary.applied = true;
    /* The operation log borrows its target string. A static scope label stays
     * valid after the review is freed; individual keys remain in review rows. */
    if (review->summary.change_count != 0U)
        (void)umi_application_operation_log_record(&runtime->operations,
            UMI_APPLICATION_OPERATION_CONTEXT_CHANGE, "context-links", UMI_STATUS_OK);
    return UMI_STATUS_OK;
}

void umi_application_context_review_destroy(UmiApplicationContextReview *review)
{
    /* Destruction releases only the review; borrowed runtime objects survive. */
    free(review);
}
