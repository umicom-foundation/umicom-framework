/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_context_review.c
 * PURPOSE: Exercise context transactions at validation, ownership and publication boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/runtime/context_review.h"
#include "umicom/application/experience_catalogue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Checks deliberately remain active when NDEBUG is defined. */
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; } } while (0)

/* Compare semantic fields rather than ABI padding, whose bytes are not part
 * of the public C value contract. Both arguments are validated fixtures. */
static int stores_equal(const UmiApplicationContextBindingStore *left,
    const UmiApplicationContextBindingStore *right)
{
    if (left->structure_size != right->structure_size || left->revision != right->revision ||
        left->entry_count != right->entry_count) return 0;
    for (size_t index = 0U; index < left->entry_count; ++index)
        if (left->entries[index].revision != right->entries[index].revision ||
            strcmp(left->entries[index].group_id, right->entries[index].group_id) != 0 ||
            strcmp(left->entries[index].value, right->entries[index].value) != 0) return 0;
    return 1;
}

static int run_case(const char *scenario, UmiApplicationWorkspaceRuntime *runtime,
    UmiUiWorkbench *workbench)
{
    UmiApplicationContextReview *review = NULL;
    UmiApplicationContextReviewSummary summary;
    UmiApplicationContextReviewRow row;
    UmiApplicationContextChange changes[2] = {
        { UMI_APPLICATION_CONTEXT_SET, "linked.first", "new value" },
        { UMI_APPLICATION_CONTEXT_SET, "linked.second", "second value" }
    };
    UmiUiContextStore *ui = umi_ui_workbench_context(workbench);
    UmiUiContextSnapshot snapshot;
    uint64_t revision;
    CHECK(umi_application_workspace_runtime_set_context(runtime, "linked.first", "old value") == UMI_STATUS_OK);
    revision = runtime->contexts.revision;

    if (strcmp(scenario, "cancel") == 0 || strcmp(scenario, "unbound") == 0) {
        uint64_t ui_revision = umi_ui_context_revision(ui);
        if (strcmp(scenario, "unbound") == 0)
            umi_application_workspace_runtime_unbind_workbench(runtime);
        CHECK(umi_application_context_review_prepare(runtime, changes, 2U, &review) == UMI_STATUS_OK);
        if (strcmp(scenario, "unbound") == 0) {
            CHECK(umi_application_context_review_summary(review, &summary) == UMI_STATUS_OK);
            CHECK(!summary.has_workbench && summary.expected_ui_revision == 0U);
            CHECK(umi_application_context_review_apply(runtime, review) == UMI_STATUS_OK);
            CHECK(runtime->contexts.entry_count == 2U && runtime->contexts.revision == revision + 1U);
        } else {
            /* Cancellation destroys only the proposal, leaving both live
             * stores and their revision tokens unchanged. */
            CHECK(runtime->contexts.entry_count == 1U && runtime->contexts.revision == revision);
        }
        umi_application_context_review_destroy(review);
        CHECK(umi_ui_context_revision(ui) == ui_revision);
        CHECK(umi_ui_context_get(ui, "linked.first", &snapshot) == UMI_STATUS_OK);
        CHECK(strcmp(snapshot.string_value, "old value") == 0);
        return 0;
    }
    if (strcmp(scenario, "invalid") == 0) {
        UmiApplicationContextReview *sentinel = (UmiApplicationContextReview *)runtime;
        CHECK(umi_application_context_review_prepare(runtime, changes,
            UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS + 1U, &sentinel) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(sentinel == (UmiApplicationContextReview *)runtime);
        changes[1] = changes[0];
        CHECK(umi_application_context_review_prepare(runtime, changes, 2U, &sentinel) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(sentinel == (UmiApplicationContextReview *)runtime);
        changes[0].operation = (UmiApplicationContextChangeKind)99;
        CHECK(umi_application_context_review_prepare(runtime, changes, 1U, &sentinel) == UMI_STATUS_INVALID_ARGUMENT);
        changes[0].operation = UMI_APPLICATION_CONTEXT_SET;
        memset(changes[0].value, 'x', sizeof(changes[0].value));
        CHECK(umi_application_context_review_prepare(runtime, changes, 1U, &sentinel) == UMI_STATUS_INVALID_ARGUMENT);
        memset(changes[0].group_id, 'x', sizeof(changes[0].group_id));
        CHECK(umi_application_context_review_prepare(runtime, changes, 1U, &sentinel) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(sentinel == (UmiApplicationContextReview *)runtime);
        CHECK(runtime->contexts.revision == revision);
        return 0;
    }
    if (strcmp(scenario, "malformed_store") == 0) {
        UmiApplicationContextBindingStore original = runtime->contexts;
        runtime->contexts.entry_count = UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS + 1U;
        CHECK(umi_application_context_review_prepare(runtime, changes, 2U, &review) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_application_context_binding_get(&runtime->contexts, "linked.first") == NULL);
        runtime->contexts = original;
        runtime->contexts.entries[1] = runtime->contexts.entries[0];
        runtime->contexts.entry_count = 2U;
        CHECK(umi_application_context_binding_validate(&runtime->contexts) == UMI_STATUS_ALREADY_EXISTS);
        runtime->contexts = original;
        memset(runtime->contexts.entries[0].value, 'x', sizeof(runtime->contexts.entries[0].value));
        CHECK(umi_application_context_binding_validate(&runtime->contexts) == UMI_STATUS_INVALID_ARGUMENT);
        runtime->contexts = original;
        return 0;
    }
    if (strcmp(scenario, "bounded_text") == 0) {
        char long_text[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY + 1U];
        UmiApplicationContextBindingStore original = runtime->contexts;
        memset(long_text, 'x', sizeof(long_text)); long_text[sizeof(long_text) - 1U] = '\0';
        CHECK(umi_application_workspace_runtime_set_context(runtime, long_text, "value") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_application_context_binding_set(&runtime->contexts, "linked.first", long_text) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(stores_equal(&original, &runtime->contexts));
        /* Inputs may alias the slot being updated; staged copies preserve both. */
        CHECK(umi_application_context_binding_set(&runtime->contexts,
            runtime->contexts.entries[0].group_id, runtime->contexts.entries[0].value) == UMI_STATUS_OK);
        CHECK(strcmp(runtime->contexts.entries[0].value, "old value") == 0);
        return 0;
    }
    if (strcmp(scenario, "entry_exhaustion") == 0) {
        runtime->contexts.entries[0].revision = UINT64_MAX;
        CHECK(umi_application_context_review_prepare(runtime, changes, 2U, &review) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(runtime->contexts.revision == revision);
        CHECK(umi_application_context_binding_set(&runtime->contexts, "linked.first", "x") == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(runtime->contexts.entries[0].value, "old value") == 0);
        return 0;
    }
    if (strcmp(scenario, "exhaustion") == 0) {
        runtime->contexts.revision = UINT64_MAX;
        CHECK(umi_application_context_review_prepare(runtime, changes, 2U, &review) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_application_workspace_runtime_clear_context(runtime, "linked.first") == UMI_STATUS_CAPACITY_EXCEEDED);
        runtime->contexts.revision = UINT64_MAX - 1U;
        CHECK(umi_application_context_review_prepare(runtime, changes, 2U, &review) == UMI_STATUS_OK);
        CHECK(umi_application_context_review_apply(runtime, review) == UMI_STATUS_OK);
        CHECK(runtime->contexts.revision == UINT64_MAX);
        umi_application_context_review_destroy(review); review = NULL;
        /* A no-op may be reviewed at the maximum revision without wrapping. */
        CHECK(umi_application_context_review_prepare(runtime, NULL, 0U, &review) == UMI_STATUS_OK);
        CHECK(umi_application_context_review_apply(runtime, review) == UMI_STATUS_OK);
        CHECK(runtime->contexts.revision == UINT64_MAX);
        umi_application_context_review_destroy(review);
        return 0;
    }
    if (strcmp(scenario, "replace_full") == 0) {
        char key[64];
        for (size_t index = 1U; index < UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS; ++index) {
            (void)snprintf(key, sizeof(key), "filled.%zu", index);
            CHECK(umi_application_workspace_runtime_set_context(runtime, key, "value") == UMI_STATUS_OK);
        }
        /* SET comes first, but REMOVE must free capacity before it is staged. */
        changes[0] = changes[1];
        changes[1] = (UmiApplicationContextChange){UMI_APPLICATION_CONTEXT_REMOVE, "linked.first", ""};
        CHECK(umi_application_context_review_prepare(runtime, changes, 2U, &review) == UMI_STATUS_OK);
        CHECK(umi_application_context_review_apply(runtime, review) == UMI_STATUS_OK);
        CHECK(runtime->contexts.entry_count == UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS);
        CHECK(umi_application_context_binding_get(&runtime->contexts, "linked.first") == NULL);
        CHECK(umi_ui_context_get(ui, "linked.second", &snapshot) == UMI_STATUS_OK);
        CHECK(strcmp(snapshot.string_value, "second value") == 0);
        umi_application_context_review_destroy(review);
        return 0;
    }
    if (strcmp(scenario, "missing_remove") == 0) {
        changes[1].operation = UMI_APPLICATION_CONTEXT_REMOVE;
        CHECK(umi_application_context_review_prepare(runtime, changes, 2U, &review) == UMI_STATUS_NOT_FOUND);
        CHECK(runtime->contexts.revision == revision);
        CHECK(strcmp(runtime->contexts.entries[0].value, "old value") == 0);
        return 0;
    }
    if (strcmp(scenario, "ui_capacity") == 0 || strcmp(scenario, "projection_capacity") == 0) {
        char key[64];
        for (size_t index = 0U; umi_ui_context_count(ui) < UMI_UI_CONTEXT_MAX; ++index) {
            (void)snprintf(key, sizeof(key), "occupied.%zu", index);
            CHECK(umi_ui_context_set_integer(ui, key, 7) == UMI_STATUS_OK);
        }
        uint64_t ui_revision = umi_ui_context_revision(ui);
        UmiApplicationContextBindingStore original = runtime->contexts;
        if (strcmp(scenario, "projection_capacity") == 0) {
            UmiApplicationContextBindingStore proposed = original;
            CHECK(umi_application_context_binding_set(&proposed, "linked.first", "new value") == UMI_STATUS_OK);
            CHECK(umi_application_context_binding_set(&proposed, "linked.second", "second value") == UMI_STATUS_OK);
            CHECK(umi_application_context_binding_apply_to_ui(&proposed, ui) == UMI_STATUS_CAPACITY_EXCEEDED);
        } else {
            CHECK(umi_application_context_review_prepare(runtime, changes, 2U, &review) == UMI_STATUS_OK);
            CHECK(umi_application_context_review_apply(runtime, review) == UMI_STATUS_CAPACITY_EXCEEDED);
            umi_application_context_review_destroy(review);
            CHECK(umi_application_workspace_runtime_set_context(runtime, "linked.second", "x") == UMI_STATUS_CAPACITY_EXCEEDED);
        }
        CHECK(stores_equal(&original, &runtime->contexts));
        CHECK(umi_ui_context_revision(ui) == ui_revision);
        CHECK(umi_ui_context_get(ui, "linked.first", &snapshot) == UMI_STATUS_OK);
        CHECK(strcmp(snapshot.string_value, "old value") == 0);
        return 0;
    }
    if (strcmp(scenario, "empty") == 0) {
        uint64_t ui_revision = umi_ui_context_revision(ui);
        CHECK(umi_application_context_review_prepare(runtime, NULL, 0U, &review) == UMI_STATUS_OK);
        CHECK(umi_application_context_review_apply(runtime, review) == UMI_STATUS_OK);
        CHECK(runtime->contexts.revision == revision && umi_ui_context_revision(ui) == ui_revision);
        umi_application_context_review_destroy(review);
        return 0;
    }
    if (strcmp(scenario, "ui_divergence") == 0) {
        CHECK(umi_ui_context_set_integer(ui, "linked.first", 42) == UMI_STATUS_OK);
    }
    if (strcmp(scenario, "ui_missing_remove") == 0) {
        CHECK(umi_ui_context_unset(ui, "linked.first") == UMI_STATUS_OK);
        changes[0].operation = UMI_APPLICATION_CONTEXT_REMOVE;
        memset(changes[0].value, 'x', sizeof(changes[0].value));
    }
    CHECK(umi_application_context_review_prepare(runtime, changes, 2U, &review) == UMI_STATUS_OK);
    CHECK(umi_application_context_review_summary(review, &summary) == UMI_STATUS_OK);
    CHECK(summary.change_count == 2U && !summary.applied);
    CHECK(umi_application_context_review_row(review, 0U, &row) == UMI_STATUS_OK);
    CHECK(row.existed && strcmp(row.previous_value, "old value") == 0);
    if (strcmp(scenario, "ui_divergence") == 0) {
        CHECK(summary.ui_difference_count == 1U);
        CHECK(row.ui_existed && !row.cache_matches_ui);
        CHECK(row.ui_previous.kind == UMI_UI_CONTEXT_INTEGER && row.ui_previous.integer_value == 42);
    }
    if (strcmp(scenario, "ui_missing_remove") == 0) CHECK(row.change.value[0] == '\0');
    CHECK(umi_application_context_review_row(review, 2U, &row) == UMI_STATUS_NOT_FOUND);
    if (strcmp(scenario, "stale_cache") == 0) {
        /* Public value stores can be edited incorrectly without incrementing
         * their revision. Compare contents as well as the revision token. */
        strcpy(runtime->contexts.entries[0].value, "external edit");
        CHECK(umi_application_context_review_apply(runtime, review) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(scenario, "stale_ui") == 0) {
        CHECK(umi_ui_context_set_boolean(ui, "external", 1) == UMI_STATUS_OK);
        CHECK(umi_application_context_review_apply(runtime, review) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(scenario, "session") == 0) {
        CHECK(umi_application_session_set_layout_locked(&runtime->session, true) == UMI_STATUS_OK);
        CHECK(umi_application_context_review_apply(runtime, review) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(scenario, "owner") == 0) {
        UmiApplicationWorkspaceRuntime *other = malloc(sizeof(*other));
        CHECK(other != NULL); *other = *runtime;
        CHECK(umi_application_context_review_apply(other, review) == UMI_STATUS_INVALID_STATE);
        CHECK(other->contexts.revision == revision);
        free(other);
    } else if (strcmp(scenario, "detach") == 0) {
        umi_application_workspace_runtime_unbind_workbench(runtime);
        CHECK(umi_application_context_review_apply(runtime, review) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(scenario, "ui_missing_remove") == 0) {
        CHECK(umi_application_context_review_apply(runtime, review) == UMI_STATUS_NOT_FOUND);
        CHECK(umi_ui_context_get(ui, "linked.second", &snapshot) == UMI_STATUS_NOT_FOUND);
    } else {
        CHECK(strcmp(scenario, "publication") == 0 || strcmp(scenario, "owned_inputs") == 0 || strcmp(scenario, "ui_divergence") == 0);
        if (strcmp(scenario, "owned_inputs") == 0) {
            /* Input and output copies may be reused before the review applies. */
            memset(changes, 0, sizeof(changes));
            memset(&row, 0, sizeof(row));
        }
        CHECK(umi_application_context_review_apply(runtime, review) == UMI_STATUS_OK);
        CHECK(runtime->contexts.revision == revision + 1U && runtime->contexts.entry_count == 2U);
        CHECK(umi_ui_context_get(ui, "linked.first", &snapshot) == UMI_STATUS_OK);
        CHECK(strcmp(snapshot.string_value, "new value") == 0);
        CHECK(umi_application_context_review_apply(runtime, review) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_application_context_review_summary(review, &summary) == UMI_STATUS_OK && summary.applied);
        CHECK(summary.added_count == 1U && summary.updated_count == 1U);
        umi_application_context_review_destroy(review);
        /* The log's label survives the owned review's destruction. */
        CHECK(strcmp(umi_application_operation_log_last(&runtime->operations)->target_id, "context-links") == 0);
        return 0;
    }
    CHECK(runtime->contexts.revision == revision);
    umi_application_context_review_destroy(review);
    return 0;
}

int main(int argc, char **argv)
{
    UmiApplicationWorkspaceRuntime *runtime = calloc(1U, sizeof(*runtime));
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    CHECK(runtime != NULL && argc == 2);
    CHECK(umi_application_workspace_runtime_init(
        umi_application_experience_catalogue_find("org.umicom.trader"), runtime) == UMI_STATUS_OK);
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("context.tests", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_application_workspace_runtime_bind_workbench(runtime, workbench) == UMI_STATUS_OK);
    int result = run_case(argv[1], runtime, workbench);
    umi_application_workspace_runtime_unbind_workbench(runtime);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    free(runtime);
    return result;
}
