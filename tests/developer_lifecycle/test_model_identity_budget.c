/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_lifecycle/test_model_identity_budget.c
 *
 * PURPOSE:
 *   Check direct context and journal revision boundaries against the actual
 *   implementations, independently of runtime event admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../support/assertions.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* Private state access is confined to this fault-injection translation unit. */
#include "../../src/developer/context.c"
#include "../../src/developer/journal.c"

static void context_budget(void)
{
    UmiDeveloperContext *context = NULL;
    assert(umi_developer_context_create(&context) == UMI_STATUS_OK);
    UmiDeveloperContextSnapshot input = {0}, output, before;
    strcpy(input.project_id, "accepted");
    context->state.revision = UINT64_MAX - 1U;
    assert(umi_developer_context_set(context, &input) == UMI_STATUS_OK);
    assert(context->state.revision == UINT64_MAX);
    memcpy(&before, &context->state, sizeof(before));
    strcpy(input.project_id, "refused");
    assert(umi_developer_context_set(context, &input) == UMI_STATUS_INVALID_STATE);
    assert(memcmp(&context->state, &before, sizeof(before)) == 0);
    UmiDeveloperContextPatch patch = {0};
    patch.struct_size = sizeof(patch);
    patch.api_version = UMI_DEVELOPER_CONTEXT_PATCH_API_VERSION;
    patch.field_mask = UMI_DEVELOPER_CONTEXT_PATCH_PROJECT;
    patch.project_id = "refused-patch";
    memset(&output, 0x5a, sizeof(output));
    unsigned char sentinel[sizeof(output)];
    memcpy(sentinel, &output, sizeof(output));
    assert(umi_developer_context_patch(context, &patch, &output) == UMI_STATUS_INVALID_STATE);
    assert(memcmp(&output, sentinel, sizeof(output)) == 0);
    assert(memcmp(&context->state, &before, sizeof(before)) == 0);
    umi_developer_context_destroy(context);
}

static void journal_budget(void)
{
    for (unsigned scenario = 0U; scenario < 4U; ++scenario) {
        UmiDeveloperJournal *journal = NULL;
        assert(umi_developer_journal_create(&journal) == UMI_STATUS_OK);
        assert(umi_developer_journal_append(journal, UMI_DEVELOPER_EVENT_PROGRESS,
            "retained", "Retained event", NULL) == UMI_STATUS_OK);
        if (scenario == 0U) journal->next_sequence = UINT64_MAX;
        if (scenario == 1U) journal->revision = UINT64_MAX;
        if (scenario >= 2U) {
            journal->count = UMI_DEVELOPER_JOURNAL_CAPACITY;
            journal->dropped_count = scenario == 2U ? UINT64_MAX : UINT64_MAX - 1U;
        }
        UmiDeveloperJournal before;
        memcpy(&before, journal, sizeof(before));
        size_t bytes = UMI_DEVELOPER_JOURNAL_CAPACITY * sizeof(*journal->entries);
        void *entries = malloc(bytes);
        assert(entries != NULL);
        memcpy(entries, journal->entries, bytes);
        UmiDeveloperJournalEntry output, sentinel;
        memset(&output, 0x5a, sizeof(output)); memcpy(&sentinel, &output, sizeof(output));
        UmiStatus status = umi_developer_journal_append(journal, UMI_DEVELOPER_EVENT_PROGRESS,
            "new", "New event", &output);
        if (scenario == 3U) {
            assert(status == UMI_STATUS_OK && journal->dropped_count == UINT64_MAX);
            assert(journal->next_sequence == before.next_sequence + 1U);
            assert(strcmp(output.operation_id, "new") == 0);
        } else {
            assert(status == UMI_STATUS_INVALID_STATE);
            assert(memcmp(journal, &before, sizeof(before)) == 0);
            assert(memcmp(journal->entries, entries, bytes) == 0);
            assert(memcmp(&output, &sentinel, sizeof(output)) == 0);
        }
        free(entries); umi_developer_journal_destroy(journal);
    }
    UmiDeveloperJournal *journal = NULL;
    assert(umi_developer_journal_create(&journal) == UMI_STATUS_OK);
    journal->next_sequence = UINT64_MAX - 1U;
    journal->revision = UINT64_MAX - 1U;
    UmiDeveloperJournalEntry entry;
    assert(umi_developer_journal_append(journal, UMI_DEVELOPER_EVENT_PROGRESS,
        "last", "Last usable identity", &entry) == UMI_STATUS_OK);
    assert(entry.sequence == UINT64_MAX - 1U && journal->next_sequence == UINT64_MAX);
    assert(journal->revision == UINT64_MAX);
    UmiDeveloperJournal before;
    memcpy(&before, journal, sizeof(before));
    assert(umi_developer_journal_clear_checked(journal) == UMI_STATUS_INVALID_STATE);
    assert(memcmp(&before, journal, sizeof(before)) == 0);
    umi_developer_journal_clear(journal);
    assert(memcmp(&before, journal, sizeof(before)) == 0);
    assert(umi_developer_journal_clear_checked(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    journal->revision = UINT64_MAX - 1U;
    assert(umi_developer_journal_clear_checked(journal) == UMI_STATUS_OK);
    assert(journal->count == 0U && journal->dropped_count == 0U);
    assert(journal->revision == UINT64_MAX && journal->next_sequence == UINT64_MAX);
    umi_developer_journal_destroy(journal);
}

int main(void)
{
    context_budget();
    journal_budget();
    return 0;
}
