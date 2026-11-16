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
#include "umicom/base/snapshot_validation.h"
#include "umicom/ai/privacy_policy.h"
#include "umicom/ai/session.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_CONVERSATION_ARCHIVE_CAPACITY 16U
/* More than the maximum escaped representation of the fixed-capacity record. */
#define UMI_AI_CONVERSATION_FILE_MAX_BYTES (1024U * 1024U)

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

/* Validate complete inline text, active message roles/count and classification.
 * Optional conversation ID, message name/text may be empty; unused message slots
 * are ignored. Input revision and nonzero approval flags remain compatible. */
UmiStatus UmiAiConversationRecordValidate(const UmiAiConversationRecord *record);
/* Check bounded storage, unique IDs and nonzero ordered revision identities.
 * NULL returns INVALID_ARGUMENT; corrupt stored state returns INVALID_STATE.
 * These checks do not synchronize access or validate arbitrary pointers. */
UmiStatus UmiAiConversationArchiveValidate(const UmiAiConversationArchive *archive);
/* Capture count and owner revision without copying records or touching disk.
 * Output stays unchanged on failure. Direct archive changes have their own
 * identity, independent of any containing AuthorEngine service revision. */
UmiStatus UmiAiConversationArchiveCapture(const UmiAiConversationArchive *archive,
    UmiSnapshotCapture *out_capture);
/* Expected zero inserts only if absent. A nonzero expected revision must match
 * this archive's selected record, otherwise BUSY. The input record's revision is
 * not assigned to storage; successful publication gives it a fresh owner revision.
 * Refused updates leave every record, count and revision unchanged. */
UmiStatus UmiAiConversationArchiveUpsertIfRevision(UmiAiConversationArchive *archive,
    const UmiAiConversationRecord *record, uint64_t expected_revision);
/* Expected revision must be nonzero. Missing IDs return NOT_FOUND; stale IDs
 * return BUSY. Success preserves other records' identities and advances the owner.
 * Both conditional and compatibility writes refuse exhausted owner revisions. */
UmiStatus UmiAiConversationArchiveRemoveIfRevision(UmiAiConversationArchive *archive,
    const char *session_id, uint64_t expected_revision);


/* A local operation receipt, not an authenticated execution record. The v1
 * conversation file stores the new session but does not persist this lineage.
 * Keep this receipt separately if the host needs a lasting parent relationship. */
typedef struct UmiAiConversationBranchReceipt {
    char source_session_id[UMI_AI_ID_CAPACITY];
    char destination_session_id[UMI_AI_ID_CAPACITY];
    uint64_t source_revision;
    uint64_t destination_revision;
    size_t message_count;
} UmiAiConversationBranchReceipt;
/* Create a new in-memory session from the first message_count source messages.
 * Zero selects an empty branch; a count above the source extent is refused.
 * Match a nonzero source record revision. Destination must be a new nonempty
 * bounded ID; an existing destination returns ALREADY_EXISTS, even the source
 * itself. The title is explicit and is never silently truncated.
 *
 * Keep provider/model/workspace and classification, assign the supplied creation
 * time to both new timestamps, and clear remote_approved/persistence_approved.
 * Copy visible message name/text and roles without unused slots or trailing
 * bytes beyond terminators. Historical roles do not prove tool execution or
 * grant permission to submit the copied content. The host reviews later use.
 *
 * Serialize on the archive owner thread. Source and all existing records retain
 * their bytes and revisions. The archive advances once on success. Allocation,
 * stale-source, capacity, exhausted-revision and invalid-input failures preserve
 * the complete archive and output receipt. Text inputs may borrow the old output
 * receipt, but neither output nor text storage may overlap the archive itself.
 * This performs no disk I/O, provider request, model learning or active-session
 * switch. Explicit persistence and execution use the existing policy boundary. */
UmiStatus UmiAiConversationArchiveBranch(UmiAiConversationArchive *archive,
    const char *source_session_id,uint64_t expected_source_revision,
    const char *new_session_id,const char *new_title,size_t message_count,
    uint64_t created_at_ns,UmiAiConversationBranchReceipt *out_receipt);

/* Serialize archive access and retain the owner across reads/edits. Conditional
 * revisions protect delayed publication, not simultaneous cross-thread access.
 * Count returns zero for NULL/invalid owners; Validate/Capture distinguish them
 * from a valid empty archive. Find/At outputs remain unchanged on failure. */


/**
 * Initialise ai conversation record from caller-provided values so later operations
 * receive a known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
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


/* Parse exactly size bytes of the complete v1 writer format. Required metadata
 * and each declared message's role/name/text occur exactly once; order is free.
 * Empty lines, CRLF and a missing final newline are accepted. Embedded or escaped
 * NUL, signed/whitespace numeric tokens, duplicate/missing/out-of-count fields
 * and unknown fields return PARSE_ERROR. Field overflow may return CAPACITY_EXCEEDED.
 * Input is limited to FILE_MAX_BYTES and may borrow the old output's bytes.
 * Every failure preserves the complete output. Parsing allocates staged storage,
 * performs no I/O and does not grant persistence/network permission. */
UmiStatus UmiAiConversationRecordParse(const void *bytes, size_t size,
    UmiAiConversationRecord *out_record);
/* Read an explicitly chosen ordinary absolute regular file under a caller limit
 * from 1 through FILE_MAX_BYTES. Reuses UmiInputFileRead: final links/devices are
 * refused; parent aliases may be followed and I/O may block. This is not directory
 * confinement. The same open handle is checked for observable changes. Every
 * read/parse failure preserves output. Prefer a worker for interactive callers.
 * The legacy load_record retains relative-path/link behavior and reads the whole
 * file before applying the format-size bound, but shares transactional parsing. */
UmiStatus UmiAiConversationArchiveLoadBounded(const char *absolute_path,
    size_t maximum_bytes, UmiAiConversationRecord *out_record);

#ifdef __cplusplus
}
#endif

#endif
