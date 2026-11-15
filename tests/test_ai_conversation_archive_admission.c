/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_conversation_archive_admission.c
 *
 * PURPOSE:
 *   Check bounded conversation fields, compatibility values and unchanged archive reads after refused input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "ai/conversation_archive_fixture.h"

int main(int argc, char **argv)
{
    static const char *const modes[] = {
        "session", "provider", "model", "workspace", "title", "conversation",
        "message-name", "message-text", "empty-session", "empty-provider",
        "empty-model", "empty-workspace", "empty-title", "optional-empty",
        "last-terminators", "unused-message", "role-low", "role-high", "count",
        "classification-low", "classification-high", "input-revision-zero",
        "input-revision-max", "approval-flags", "query", "null-record"
    };
    UmiAiConversationArchive *archive = NULL;
    UmiAiConversationRecord *record = archive_record_fixture("session.fixture");
    UmiAiConversationRecord *stored = calloc(1U, sizeof *stored);
    UmiAiConversationRecord *output = calloc(1U, sizeof *output);
    UmiSnapshotCapture before, after;
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    int known = 0;
    assert(argc == 2 && stored != NULL && output != NULL);
    for (size_t i = 0U; i < sizeof modes / sizeof modes[0]; ++i)
        if (strcmp(argv[1], modes[i]) == 0) known = 1;
    assert(known);
    assert(umi_ai_conversation_archive_create(&archive) == UMI_STATUS_OK);
    assert(umi_ai_conversation_archive_upsert(archive, record) == UMI_STATUS_OK);
    assert(umi_ai_conversation_archive_find(archive, "session.fixture", stored) == UMI_STATUS_OK);
    assert(UmiAiConversationArchiveCapture(archive, &before) == UMI_STATUS_OK);
#define CASE(name) (strcmp(argv[1], name) == 0)
#define FILL(field) memset((field), 'x', sizeof(field))
#define LAST(field) do { FILL(field); (field)[sizeof(field) - 1U] = '\0'; } while (0)
    if (CASE("session")) FILL(record->session.session_id);
    else if (CASE("provider")) FILL(record->session.provider_id);
    else if (CASE("model")) FILL(record->session.model_id);
    else if (CASE("workspace")) FILL(record->workspace_id);
    else if (CASE("title")) FILL(record->title);
    else if (CASE("conversation")) FILL(record->conversation.conversation_id);
    else if (CASE("message-name")) FILL(record->conversation.messages[0].name);
    else if (CASE("message-text")) FILL(record->conversation.messages[0].text);
    else if (CASE("empty-session")) record->session.session_id[0] = '\0';
    else if (CASE("empty-provider")) record->session.provider_id[0] = '\0';
    else if (CASE("empty-model")) record->session.model_id[0] = '\0';
    else if (CASE("empty-workspace")) record->workspace_id[0] = '\0';
    else if (CASE("empty-title")) record->title[0] = '\0';
    else if (CASE("role-low")) record->conversation.messages[0].role = (UmiAiRole)0;
    else if (CASE("role-high")) record->conversation.messages[0].role = (UmiAiRole)(UMI_AI_ROLE_TOOL + 1);
    else if (CASE("count")) record->conversation.message_count = UMI_AI_MAX_MESSAGES + 1U;
    else if (CASE("classification-low")) record->classification = (UmiAiDataClassification)-1;
    else if (CASE("classification-high")) record->classification = (UmiAiDataClassification)(UMI_AI_DATA_RESTRICTED + 1);
    else if (CASE("optional-empty")) {
        record->conversation.conversation_id[0] = '\0';
        record->conversation.messages[0].name[0] = '\0';
        record->conversation.messages[0].text[0] = '\0';
        expected = UMI_STATUS_OK;
    } else if (CASE("last-terminators")) {
        LAST(record->session.session_id); LAST(record->session.provider_id);
        LAST(record->session.model_id); LAST(record->workspace_id); LAST(record->title);
        LAST(record->conversation.conversation_id);
        LAST(record->conversation.messages[0].name); LAST(record->conversation.messages[0].text);
        expected = UMI_STATUS_OK;
    } else if (CASE("unused-message")) {
        memset(&record->conversation.messages[UMI_AI_MAX_MESSAGES - 1U], 0xA5,
            sizeof(record->conversation.messages[0]));
        expected = UMI_STATUS_OK;
    } else if (CASE("input-revision-zero")) {
        record->revision = 0U; expected = UMI_STATUS_OK;
    } else if (CASE("input-revision-max")) {
        record->revision = UINT64_MAX; expected = UMI_STATUS_OK;
    } else if (CASE("approval-flags")) {
        record->remote_approved = 7; record->persistence_approved = -1;
        expected = UMI_STATUS_OK;
    } else if (CASE("query")) {
        char bad[UMI_AI_ID_CAPACITY];
        const char *queries[3] = {NULL, "", bad};
        memset(bad, 'x', sizeof bad);
        memset(output, 0xA5, sizeof *output);
        memcpy(record, output, sizeof *record);
        for (size_t i = 0U; i < 3U; ++i) {
            assert(umi_ai_conversation_archive_find(archive, queries[i], output) == UMI_STATUS_INVALID_ARGUMENT);
            assert(umi_ai_conversation_archive_remove(archive, queries[i]) == UMI_STATUS_INVALID_ARGUMENT);
            assert(memcmp(record, output, sizeof *output) == 0);
        }
        assert(umi_ai_conversation_archive_find(archive, "missing", output) == UMI_STATUS_NOT_FOUND);
        assert(umi_ai_conversation_archive_at(archive, SIZE_MAX, output) == UMI_STATUS_NOT_FOUND);
        assert(memcmp(record, output, sizeof *output) == 0);
        goto rejected;
    } else if (CASE("null-record")) {
        assert(UmiAiConversationRecordValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        assert(umi_ai_conversation_archive_upsert(archive, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiAiConversationArchiveUpsertIfRevision(archive, NULL, stored->revision) == UMI_STATUS_INVALID_ARGUMENT);
        goto rejected;
    }
    assert(UmiAiConversationRecordValidate(record) == expected);
    assert(umi_ai_conversation_archive_upsert(archive, record) == expected);
    if (expected == UMI_STATUS_OK) {
        assert(umi_ai_conversation_archive_find(archive, record->session.session_id, output) == UMI_STATUS_OK);
        assert(output->revision == before.revision + 1U);
        record->revision = output->revision;
        assert(memcmp(record, output, sizeof *record) == 0);
        assert(UmiAiConversationArchiveCapture(archive, &after) == UMI_STATUS_OK);
        assert(after.revision == before.revision + 1U);
        assert(after.count == before.count + (CASE("last-terminators") ? 1U : 0U));
        goto cleanup;
    }
    assert(UmiAiConversationArchiveUpsertIfRevision(archive, record, stored->revision) == expected);
rejected:
    assert(UmiAiConversationArchiveCapture(archive, &after) == UMI_STATUS_OK);
    assert(after.count == before.count && after.revision == before.revision);
    assert(umi_ai_conversation_archive_find(archive, "session.fixture", output) == UMI_STATUS_OK);
    assert(memcmp(stored, output, sizeof *stored) == 0);
cleanup:
    assert(UmiAiConversationArchiveValidate(archive) == UMI_STATUS_OK);
    umi_ai_conversation_archive_destroy(archive);
    free(record); free(stored); free(output);
    return 0;
#undef LAST
#undef FILL
#undef CASE
}
