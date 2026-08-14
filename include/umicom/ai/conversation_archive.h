/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/conversation_archive.h
 *
 * PURPOSE:
 *   Store complete AI sessions and conversations in bounded memory and persist
 *   one selected record using a portable, atomic, versioned text format.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Conversation text is escaped before it enters the archive file, so newlines,
 * percent characters and other control bytes round-trip without truncation.
 */
#ifndef UMICOM_AI_CONVERSATION_ARCHIVE_H
#define UMICOM_AI_CONVERSATION_ARCHIVE_H

#include "umicom/ai/conversation.h"
#include "umicom/ai/privacy_policy.h"
#include "umicom/ai/session.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_CONVERSATION_ARCHIVE_CAPACITY 16U

typedef struct UmiAiConversationRecord {
    UmiAiSession session;
    UmiAiConversation conversation;
    char workspace_id[UMI_AI_ID_CAPACITY];
    char title[UMI_AI_SMALL_TEXT_CAPACITY];
    UmiAiDataClassification classification;
    int remote_approved;
    int persistence_approved;
    uint64_t updated_at_ns;
    uint64_t revision;
} UmiAiConversationRecord;

typedef struct UmiAiConversationArchive UmiAiConversationArchive;

UmiStatus umi_ai_conversation_record_init(
    UmiAiConversationRecord *record,
    const char *session_id,
    const char *provider_id,
    const char *model_id,
    const char *workspace_id,
    const char *title,
    uint64_t created_at_ns);
UmiStatus umi_ai_conversation_archive_create(
    UmiAiConversationArchive **out_archive);
void umi_ai_conversation_archive_destroy(UmiAiConversationArchive *archive);
UmiStatus umi_ai_conversation_archive_upsert(
    UmiAiConversationArchive *archive,
    const UmiAiConversationRecord *record);
UmiStatus umi_ai_conversation_archive_find(
    const UmiAiConversationArchive *archive,
    const char *session_id,
    UmiAiConversationRecord *out_record);
UmiStatus umi_ai_conversation_archive_at(
    const UmiAiConversationArchive *archive,
    size_t index,
    UmiAiConversationRecord *out_record);
UmiStatus umi_ai_conversation_archive_remove(
    UmiAiConversationArchive *archive,
    const char *session_id);
size_t umi_ai_conversation_archive_count(
    const UmiAiConversationArchive *archive);
UmiStatus umi_ai_conversation_archive_save_record(
    const UmiAiConversationRecord *record,
    const UmiAiPrivacyPolicy *privacy,
    const char *path);
UmiStatus umi_ai_conversation_archive_load_record(
    const char *path,
    UmiAiConversationRecord *out_record);

#ifdef __cplusplus
}
#endif

#endif
