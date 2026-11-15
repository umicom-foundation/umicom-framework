/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_conversation_archive_revision.c
 *
 * PURPOSE:
 *   Verify stale session edits, exhausted write identities, opaque-owner invariants and independent archive captures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "ai/conversation_archive_fixture.h"
/* Compile the real owner to inject unreachable-in-practice limits and corrupt
 * private fields. Production callers continue to see an opaque archive. */
#include "../src/ai/conversation_archive.c"

static UmiSnapshotCapture capture(UmiAiConversationArchive *archive)
{
    UmiSnapshotCapture value;
    assert(UmiAiConversationArchiveCapture(archive, &value) == UMI_STATUS_OK);
    return value;
}

int main(int argc, char **argv)
{
    static const char *const modes[] = {
        "conditional-insert", "stale-replace", "missing-replace", "stale-remove",
        "remove-neighbour", "saturation-upsert", "saturation-remove", "last-revision",
        "capacity", "count", "zero-revision", "zero-record", "ahead-record",
        "duplicate", "stored-text", "capture", "missing-remove", "unrelated-edit"
    };
    UmiAiConversationArchive *archive = NULL;
    UmiAiConversationArchive *bytes_before;
    UmiAiConversationRecord *first = archive_record_fixture("session.first");
    UmiAiConversationRecord *second = archive_record_fixture("session.second");
    UmiAiConversationRecord *output = calloc(1U, sizeof *output);
    UmiAiConversationRecord *output_before = calloc(1U, sizeof *output_before);
    UmiSnapshotCapture before, after;
    int known = 0;
    assert(argc == 2 && output != NULL && output_before != NULL);
    for (size_t i = 0U; i < sizeof modes / sizeof modes[0]; ++i)
        if (strcmp(argv[1], modes[i]) == 0) known = 1;
    assert(known);
    assert(umi_ai_conversation_archive_create(&archive) == UMI_STATUS_OK);
    bytes_before = malloc(sizeof *archive);
    assert(bytes_before != NULL);
    assert(UmiAiConversationArchiveUpsertIfRevision(archive, first, 0U) == UMI_STATUS_OK);
    assert(UmiAiConversationArchiveUpsertIfRevision(archive, second, 0U) == UMI_STATUS_OK);
    assert(umi_ai_conversation_archive_find(archive, first->session.session_id, first) == UMI_STATUS_OK);
    assert(umi_ai_conversation_archive_find(archive, second->session.session_id, second) == UMI_STATUS_OK);
    before = capture(archive);
#define CASE(name) (strcmp(argv[1], name) == 0)
    if (CASE("count")) archive->count = UMI_AI_CONVERSATION_ARCHIVE_CAPACITY + 1U;
    else if (CASE("zero-revision")) archive->revision = 0U;
    else if (CASE("zero-record")) archive->records[0].revision = 0U;
    else if (CASE("ahead-record")) archive->records[0].revision = archive->revision + 1U;
    else if (CASE("duplicate")) strcpy(archive->records[1].session.session_id, first->session.session_id);
    else if (CASE("stored-text")) memset(archive->records[0].conversation.messages[0].text,
        'x', sizeof archive->records[0].conversation.messages[0].text);
    if (CASE("count") || CASE("zero-revision") || CASE("zero-record") ||
        CASE("ahead-record") || CASE("duplicate") || CASE("stored-text")) {
        memcpy(bytes_before, archive, sizeof *archive);
        memset(output, 0xA5, sizeof *output); memcpy(output_before, output, sizeof *output);
        after = (UmiSnapshotCapture){87U, 99U};
        assert(UmiAiConversationArchiveValidate(archive) == UMI_STATUS_INVALID_STATE);
        assert(UmiAiConversationArchiveCapture(archive, &after) == UMI_STATUS_INVALID_STATE);
        assert(after.count == 87U && after.revision == 99U);
        assert(umi_ai_conversation_archive_count(archive) == 0U);
        assert(umi_ai_conversation_archive_find(archive, "session.first", output) == UMI_STATUS_INVALID_STATE);
        assert(umi_ai_conversation_archive_at(archive, 0U, output) == UMI_STATUS_INVALID_STATE);
        assert(memcmp(output, output_before, sizeof *output) == 0);
        assert(umi_ai_conversation_archive_upsert(archive, first) == UMI_STATUS_INVALID_STATE);
        assert(UmiAiConversationArchiveUpsertIfRevision(archive, first, first->revision) == UMI_STATUS_INVALID_STATE);
        assert(umi_ai_conversation_archive_remove(archive, "session.first") == UMI_STATUS_INVALID_STATE);
        assert(UmiAiConversationArchiveRemoveIfRevision(archive, "session.first", first->revision) == UMI_STATUS_INVALID_STATE);
        assert(memcmp(archive, bytes_before, sizeof *archive) == 0);
        goto cleanup;
    }
    if (CASE("saturation-upsert") || CASE("saturation-remove")) archive->revision = UINT64_MAX;
    if (CASE("last-revision")) archive->revision = UINT64_MAX - 1U;
    before = capture(archive);
    memcpy(bytes_before, archive, sizeof *archive);
    if (CASE("conditional-insert")) {
        assert(UmiAiConversationArchiveUpsertIfRevision(archive, first, 0U) == UMI_STATUS_BUSY);
        assert(memcmp(archive, bytes_before, sizeof *archive) == 0);
        strcpy(first->session.session_id, "session.third");
        assert(UmiAiConversationArchiveUpsertIfRevision(archive, first, 0U) == UMI_STATUS_OK);
        after = capture(archive);
        assert(after.count == 3U && after.revision == before.revision + 1U);
    } else if (CASE("stale-replace")) {
        uint64_t stale = first->revision;
        strcpy(first->title, "Accepted current title");
        assert(UmiAiConversationArchiveUpsertIfRevision(archive, first, stale) == UMI_STATUS_OK);
        memcpy(bytes_before, archive, sizeof *archive);
        strcpy(first->title, "Must not overwrite the accepted title");
        assert(UmiAiConversationArchiveUpsertIfRevision(archive, first, stale) == UMI_STATUS_BUSY);
        assert(memcmp(archive, bytes_before, sizeof *archive) == 0);
        assert(umi_ai_conversation_archive_find(archive, first->session.session_id, output) == UMI_STATUS_OK);
        assert(strcmp(output->title, "Accepted current title") == 0 && output->revision > stale);
    } else if (CASE("missing-replace")) {
        strcpy(first->session.session_id, "session.missing");
        assert(UmiAiConversationArchiveUpsertIfRevision(archive, first, first->revision) == UMI_STATUS_BUSY);
        assert(memcmp(archive, bytes_before, sizeof *archive) == 0);
    } else if (CASE("stale-remove")) {
        assert(UmiAiConversationArchiveRemoveIfRevision(archive, first->session.session_id, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiAiConversationArchiveRemoveIfRevision(archive, first->session.session_id, first->revision + 1U) == UMI_STATUS_BUSY);
        assert(memcmp(archive, bytes_before, sizeof *archive) == 0);
    } else if (CASE("remove-neighbour")) {
        assert(UmiAiConversationArchiveRemoveIfRevision(archive, first->session.session_id, first->revision) == UMI_STATUS_OK);
        after = capture(archive);
        assert(after.count == 1U && after.revision == before.revision + 1U);
        assert(umi_ai_conversation_archive_at(archive, 0U, output) == UMI_STATUS_OK);
        assert(memcmp(second, output, sizeof *second) == 0);
        assert(umi_ai_conversation_archive_find(archive, first->session.session_id, output) == UMI_STATUS_NOT_FOUND);
    } else if (CASE("saturation-upsert")) {
        assert(umi_ai_conversation_archive_upsert(archive, first) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(UmiAiConversationArchiveUpsertIfRevision(archive, first, first->revision) == UMI_STATUS_CAPACITY_EXCEEDED);
        strcpy(first->session.session_id, "session.third");
        assert(UmiAiConversationArchiveUpsertIfRevision(archive, first, 0U) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(memcmp(archive, bytes_before, sizeof *archive) == 0);
    } else if (CASE("saturation-remove")) {
        assert(umi_ai_conversation_archive_remove(archive, first->session.session_id) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(UmiAiConversationArchiveRemoveIfRevision(archive, first->session.session_id, first->revision) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(memcmp(archive, bytes_before, sizeof *archive) == 0);
    } else if (CASE("last-revision")) {
        assert(UmiAiConversationArchiveUpsertIfRevision(archive, first, first->revision) == UMI_STATUS_OK);
        assert(umi_ai_conversation_archive_find(archive, first->session.session_id, output) == UMI_STATUS_OK);
        assert(output->revision == UINT64_MAX && capture(archive).revision == UINT64_MAX);
        memcpy(bytes_before, archive, sizeof *archive);
        assert(UmiAiConversationArchiveUpsertIfRevision(archive, output, output->revision) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(umi_ai_conversation_archive_remove(archive, second->session.session_id) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(memcmp(archive, bytes_before, sizeof *archive) == 0);
    } else if (CASE("capacity")) {
        for (size_t i = 2U; i < UMI_AI_CONVERSATION_ARCHIVE_CAPACITY; ++i) {
            (void)snprintf(first->session.session_id, sizeof first->session.session_id, "session.%zu", i);
            assert(UmiAiConversationArchiveUpsertIfRevision(archive, first, 0U) == UMI_STATUS_OK);
        }
        memcpy(bytes_before, archive, sizeof *archive);
        strcpy(first->session.session_id, "session.overflow");
        assert(UmiAiConversationArchiveUpsertIfRevision(archive, first, 0U) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(memcmp(archive, bytes_before, sizeof *archive) == 0);
        strcpy(second->title, "Replacement at capacity");
        assert(UmiAiConversationArchiveUpsertIfRevision(archive, second, second->revision) == UMI_STATUS_OK);
        assert(capture(archive).count == UMI_AI_CONVERSATION_ARCHIVE_CAPACITY);
    } else if (CASE("capture")) {
        after = (UmiSnapshotCapture){87U, 99U};
        assert(UmiAiConversationArchiveCapture(NULL, &after) == UMI_STATUS_INVALID_ARGUMENT);
        assert(after.count == 87U && after.revision == 99U);
        assert(UmiAiConversationArchiveCapture(archive, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiAiConversationArchiveValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        assert(umi_ai_conversation_archive_count(NULL) == 0U);
        assert(memcmp(archive, bytes_before, sizeof *archive) == 0);
    } else if (CASE("missing-remove")) {
        assert(UmiAiConversationArchiveRemoveIfRevision(archive, "session.missing", 1U) == UMI_STATUS_NOT_FOUND);
        assert(umi_ai_conversation_archive_remove(archive, "session.missing") == UMI_STATUS_NOT_FOUND);
        assert(memcmp(archive, bytes_before, sizeof *archive) == 0);
    } else if (CASE("unrelated-edit")) {
        strcpy(second->title, "An independent edit");
        assert(UmiAiConversationArchiveUpsertIfRevision(archive, second, second->revision) == UMI_STATUS_OK);
        /* A different record's newer revision does not invalidate this read. */
        strcpy(first->title, "Still based on the current first record");
        assert(UmiAiConversationArchiveUpsertIfRevision(archive, first, first->revision) == UMI_STATUS_OK);
        assert(capture(archive).revision == before.revision + 2U);
    }
    assert(UmiAiConversationArchiveValidate(archive) == UMI_STATUS_OK);
cleanup:
    umi_ai_conversation_archive_destroy(archive);
    free(bytes_before); free(first); free(second); free(output); free(output_before);
    return 0;
#undef CASE
}
