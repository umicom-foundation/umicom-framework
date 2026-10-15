/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_ai_conversation_archive.c
 *
 * PURPOSE:
 *   Verify bounded session storage and portable escaped conversation files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "value_archive/construction_cases.h"
#include <string.h>

#include "umicom/ai/conversation_archive.h"
#include "umicom/platform/filesystem.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted conversation archive value. */
static int UmiAiConversationRecordConstructionCases(void)
{
    UmiAiConversationRecord *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ai_conversation_record_init(value, "session.archive.47", "provider.local", "model.chat", "workspace.batch47", "Escaped archive", 47U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->session.session_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_conversation_record_init(value, oversized, "provider.local", "model.chat", "workspace.batch47", "Escaped archive", 47U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_conversation_record_init(value, value->session.session_id, "provider.local", "model.chat", "workspace.batch47", "Escaped archive", 47U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->session.session_id, "session.archive.47") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->session.provider_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_conversation_record_init(value, "session.archive.47", oversized, "model.chat", "workspace.batch47", "Escaped archive", 47U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_conversation_record_init(value, "session.archive.47", value->session.provider_id, "model.chat", "workspace.batch47", "Escaped archive", 47U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->session.provider_id, "provider.local") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->session.model_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_conversation_record_init(value, "session.archive.47", "provider.local", oversized, "workspace.batch47", "Escaped archive", 47U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_conversation_record_init(value, "session.archive.47", "provider.local", value->session.model_id, "workspace.batch47", "Escaped archive", 47U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->session.model_id, "model.chat") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->workspace_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_conversation_record_init(value, "session.archive.47", "provider.local", "model.chat", oversized, "Escaped archive", 47U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_conversation_record_init(value, "session.archive.47", "provider.local", "model.chat", value->workspace_id, "Escaped archive", 47U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->workspace_id, "workspace.batch47") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->title) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_conversation_record_init(value, "session.archive.47", "provider.local", "model.chat", "workspace.batch47", oversized, 47U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_conversation_record_init(value, "session.archive.47", "provider.local", "model.chat", "workspace.batch47", value->title, 47U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->title, "Escaped archive") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiAiConversationRecordConstructionCases() != 0) return 1;

    UmiAiConversationArchive *archive = NULL;
    UmiAiConversationRecord record;
    UmiAiConversationRecord loaded;
    UmiAiMessage message;
    UmiAiPrivacyPolicy privacy = umi_ai_privacy_policy_default();
    char temporary[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    assert(umi_ai_conversation_record_init(
        &record, "session.archive.47", "provider.local", "model.chat",
        "workspace.batch47", "Escaped archive", 47U) == UMI_STATUS_OK);
    assert(umi_ai_message_set(
        &message, UMI_AI_ROLE_USER, "beginner",
        "First line\nSecond line with 100% confidence = checked")
        == UMI_STATUS_OK);
    assert(umi_ai_conversation_add(&record.conversation, &message)
        == UMI_STATUS_OK);
    assert(umi_ai_conversation_archive_create(&archive) == UMI_STATUS_OK);
    assert(umi_ai_conversation_archive_upsert(archive, &record)
        == UMI_STATUS_OK);
    assert(umi_ai_conversation_archive_count(archive) == 1U);
    assert(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    assert(umi_fs_join(path, sizeof(path), temporary,
                       "umicom-ai-session-batch47.txt") == UMI_STATUS_OK);
    (void)umi_fs_remove_tree(path);
    assert(umi_ai_conversation_archive_save_record(&record, &privacy, path)
        == UMI_STATUS_OK);
    assert(umi_ai_conversation_archive_load_record(path, &loaded)
        == UMI_STATUS_OK);
    assert(strcmp(loaded.session.session_id, record.session.session_id) == 0);
    assert(loaded.conversation.message_count == 1U);
    assert(strcmp(loaded.conversation.messages[0].text, message.text) == 0);
    assert(umi_fs_remove_tree(path) == UMI_STATUS_OK);
    umi_ai_conversation_archive_destroy(archive);
    return 0;
}
