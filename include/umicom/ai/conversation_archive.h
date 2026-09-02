/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/conversation_archive.h
 *
 * PURPOSE:
 *   Store complete AI sessions and conversations in bounded memory and persist
 *   one selected record using a portable, atomic, versioned text format.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/**
 * Represent the ai conversation record data shared with callers of this public contract.
 */
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

/**
 * Represent the ai conversation archive data shared with callers of this public contract.
 */
typedef struct UmiAiConversationArchive UmiAiConversationArchive;

/**
 * Initialise ai conversation record from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_conversation_record_init(
    UmiAiConversationRecord *record,
    const char *session_id,
    const char *provider_id,
    const char *model_id,
    const char *workspace_id,
    const char *title,
    uint64_t created_at_ns);
/**
 * Initialise ai conversation archive from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_conversation_archive_create(
    UmiAiConversationArchive **out_archive);
/**
 * Release or reset state held by ai conversation archive so the same storage can be reused
 * safely.
 */
void umi_ai_conversation_archive_destroy(UmiAiConversationArchive *archive);
/**
 * Provide the ai conversation archive upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_conversation_archive_upsert(
    UmiAiConversationArchive *archive,
    const UmiAiConversationRecord *record);
/**
 * Find ai conversation archive while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ai_conversation_archive_find(
    const UmiAiConversationArchive *archive,
    const char *session_id,
    UmiAiConversationRecord *out_record);
/**
 * Find ai conversation archive while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ai_conversation_archive_at(
    const UmiAiConversationArchive *archive,
    size_t index,
    UmiAiConversationRecord *out_record);
/**
 * Remove ai conversation archive while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ai_conversation_archive_remove(
    UmiAiConversationArchive *archive,
    const char *session_id);
/**
 * Return the number of records represented by ai conversation archive without changing
 * their state.
 */
size_t umi_ai_conversation_archive_count(
    const UmiAiConversationArchive *archive);
/**
 * Provide the ai conversation archive save record operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_conversation_archive_save_record(
    const UmiAiConversationRecord *record,
    const UmiAiPrivacyPolicy *privacy,
    const char *path);
/**
 * Provide the ai conversation archive load record operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_conversation_archive_load_record(
    const char *path,
    UmiAiConversationRecord *out_record);

#ifdef __cplusplus
}
#endif

#endif
