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
#include <string.h>

#include "umicom/ai/conversation_archive.h"
#include "umicom/platform/filesystem.h"

int main(void)
{
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
