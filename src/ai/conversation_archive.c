/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/conversation_archive.c
 *
 * PURPOSE:
 *   Implement bounded conversation records plus atomic, escaped and versioned
 *   persistence for AuthorEngine Integration v2 sessions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Saving is explicit and policy checked.  The temporary file lives beside the
 * destination so the final rename is atomic on supported local filesystems.
 */
#include "umicom/ai/conversation_archive.h"
#include "umicom/base/snapshot_validation.h"
#include "umicom/base/ascii_number.h"
#include "umicom/platform/input_file.h"
#include "../base/value_construction_internal.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/atomic_file.h"
#include "umicom/platform/filesystem.h"

struct UmiAiConversationArchive {
    UmiAiConversationRecord records[UMI_AI_CONVERSATION_ARCHIVE_CAPACITY];
    size_t count;
    uint64_t revision;
};

typedef struct TextBuilder {
    char *text;
    size_t length;
    size_t capacity;
} TextBuilder;

/* Provide the copy text operation used by this module and its client applications. */
static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) return 0;
    written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiAiConversationArchive *archive,
                         const char *session_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (archive == NULL || session_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < archive->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(archive->records[index].session.session_id,
                   session_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/* Check that record satisfies its contract before another service relies on it. */
/* Complete bounded-field and opaque-owner validation replaces first-byte-only record admission before lookup and persistence. The former implementation is retained for owner review. */
#if 0
static int record_valid(const UmiAiConversationRecord *record)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || record->session.session_id[0] == '\0' ||
        record->session.provider_id[0] == '\0' ||
        record->session.model_id[0] == '\0' ||
        record->workspace_id[0] == '\0' || record->title[0] == '\0' ||
        record->conversation.message_count > UMI_AI_MAX_MESSAGES ||
        record->classification < UMI_AI_DATA_PUBLIC ||
        record->classification > UMI_AI_DATA_RESTRICTED) {
        return 0;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < record->conversation.message_count; ++index) {
        UmiAiRole role = record->conversation.messages[index].role;
        /* Apply this branch only when its contract condition is satisfied. */
        if (role < UMI_AI_ROLE_SYSTEM || role > UMI_AI_ROLE_TOOL) return 0;
    }
    /* Malformed persisted roles are rejected before an adapter can mistake
     * them for trusted system or tool messages. */
    return 1;
}
#endif
/* Reuse the Framework's bounded-field validator before any strcmp, strlen or
 * serialization reads. Optional names/text can be empty; unused message slots
 * are not part of the record until message_count makes them visible. */
UmiStatus UmiAiConversationRecordValidate(const UmiAiConversationRecord *record)
{
    static const UmiSnapshotTextField fields[] = {
        {"session_id", offsetof(UmiAiConversationRecord, session.session_id),
            sizeof(((UmiAiConversationRecord *)0)->session.session_id), 1},
        {"provider_id", offsetof(UmiAiConversationRecord, session.provider_id),
            sizeof(((UmiAiConversationRecord *)0)->session.provider_id), 1},
        {"model_id", offsetof(UmiAiConversationRecord, session.model_id),
            sizeof(((UmiAiConversationRecord *)0)->session.model_id), 1},
        {"workspace_id", offsetof(UmiAiConversationRecord, workspace_id),
            sizeof(((UmiAiConversationRecord *)0)->workspace_id), 1},
        {"title", offsetof(UmiAiConversationRecord, title),
            sizeof(((UmiAiConversationRecord *)0)->title), 1},
        {"conversation_id", offsetof(UmiAiConversationRecord, conversation.conversation_id),
            sizeof(((UmiAiConversationRecord *)0)->conversation.conversation_id), 0}
    };
    static const UmiSnapshotTextField message_fields[] = {
        {"name", offsetof(UmiAiMessage, name), sizeof(((UmiAiMessage *)0)->name), 0},
        {"text", offsetof(UmiAiMessage, text), sizeof(((UmiAiMessage *)0)->text), 0}
    };
    if (UmiSnapshotValidateTextFields(record, sizeof(*record), fields,
            sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (record->conversation.message_count > UMI_AI_MAX_MESSAGES ||
        record->classification < UMI_AI_DATA_PUBLIC ||
        record->classification > UMI_AI_DATA_RESTRICTED)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < record->conversation.message_count; ++index) {
        const UmiAiMessage *message = &record->conversation.messages[index];
        if (message->role < UMI_AI_ROLE_SYSTEM || message->role > UMI_AI_ROLE_TOOL ||
            UmiSnapshotValidateTextFields(message, sizeof(*message), message_fields,
                sizeof message_fields / sizeof message_fields[0], NULL) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static int record_valid(const UmiAiConversationRecord *record)
{
    return UmiAiConversationRecordValidate(record) == UMI_STATUS_OK;
}

/* Validate the opaque owner's bounds before traversing records. This detects
 * corrupt values, not dangling pointers, and is not synchronization. */
UmiStatus UmiAiConversationArchiveValidate(const UmiAiConversationArchive *archive)
{
    if (archive == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (archive->count > UMI_AI_CONVERSATION_ARCHIVE_CAPACITY || archive->revision == 0U)
        return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < archive->count; ++index) {
        const UmiAiConversationRecord *record = &archive->records[index];
        if (!record_valid(record) || record->revision == 0U || record->revision > archive->revision)
            return UMI_STATUS_INVALID_STATE;
        for (size_t prior = 0U; prior < index; ++prior)
            if (strcmp(record->session.session_id, archive->records[prior].session.session_id) == 0)
                return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

static int archive_id_valid(const char *id)
{
    if (id == NULL || id[0] == '\0') return 0;
    for (size_t index = 0U; index < UMI_AI_ID_CAPACITY; ++index)
        if (id[index] == '\0') return 1;
    return 0;
}

UmiStatus UmiAiConversationArchiveCapture(const UmiAiConversationArchive *archive,
    UmiSnapshotCapture *out_capture)
{
    UmiStatus status;
    if (out_capture == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiConversationArchiveValidate(archive);
    if (status != UMI_STATUS_OK) return status;
    *out_capture = (UmiSnapshotCapture){archive->count, archive->revision};
    return UMI_STATUS_OK;
}

/*
 * Initialise ai conversation record from caller-provided values so later operations
 * receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_ai_conversation_record_init(
    UmiAiConversationRecord *record,
    const char *session_id,
    const char *provider_id,
    const char *model_id,
    const char *workspace_id,
    const char *title,
    uint64_t created_at_ns)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || session_id == NULL || provider_id == NULL ||
        model_id == NULL || workspace_id == NULL || title == NULL ||
        session_id[0] == '\0' || provider_id[0] == '\0' ||
        model_id[0] == '\0' || workspace_id[0] == '\0' || title[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(record, 0, sizeof(*record));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!copy_text(record->session.session_id,
                   sizeof(record->session.session_id), session_id) ||
        !copy_text(record->session.provider_id,
                   sizeof(record->session.provider_id), provider_id) ||
        !copy_text(record->session.model_id,
                   sizeof(record->session.model_id), model_id) ||
        !copy_text(record->workspace_id, sizeof(record->workspace_id),
                   workspace_id) ||
        !copy_text(record->title, sizeof(record->title), title)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    record->session.created_at_ns = created_at_ns;
    umi_ai_conversation_init(&record->conversation, session_id);
    record->classification = UMI_AI_DATA_INTERNAL;
    record->updated_at_ns = created_at_ns;
    record->revision = 1U;
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_ai_conversation_record_init_build_value(
    UmiAiConversationRecord *record,
    const char *session_id,
    const char *provider_id,
    const char *model_id,
    const char *workspace_id,
    const char *title,
    uint64_t created_at_ns)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || session_id == NULL || provider_id == NULL ||
        model_id == NULL || workspace_id == NULL || title == NULL ||
        session_id[0] == '\0' || provider_id[0] == '\0' ||
        model_id[0] == '\0' || workspace_id[0] == '\0' || title[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(record, 0, sizeof(*record));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!copy_text(record->session.session_id,
                   sizeof(record->session.session_id), session_id) ||
        !copy_text(record->session.provider_id,
                   sizeof(record->session.provider_id), provider_id) ||
        !copy_text(record->session.model_id,
                   sizeof(record->session.model_id), model_id) ||
        !copy_text(record->workspace_id, sizeof(record->workspace_id),
                   workspace_id) ||
        !copy_text(record->title, sizeof(record->title), title)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    record->session.created_at_ns = created_at_ns;
    umi_ai_conversation_init(&record->conversation, session_id);
    record->classification = UMI_AI_DATA_INTERNAL;
    record->updated_at_ns = created_at_ns;
    record->revision = 1U;
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_ai_conversation_record_init, UmiAiConversationRecord, record,
    (UmiAiConversationRecord *record, const char *session_id, const char *provider_id, const char *model_id, const char *workspace_id, const char *title, uint64_t created_at_ns),
    umi_ai_conversation_record_init_build_value(candidate, session_id, provider_id, model_id, workspace_id, title, created_at_ns))


/*
 * Initialise ai conversation archive from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_conversation_archive_create(
    UmiAiConversationArchive **out_archive)
{
    UmiAiConversationArchive *archive;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_archive == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_archive = NULL;
    archive = (UmiAiConversationArchive *)calloc(1U, sizeof(*archive));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (archive == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    archive->revision = 1U;
    *out_archive = archive;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai conversation archive so the same storage can be reused
 * safely.
 */
void umi_ai_conversation_archive_destroy(UmiAiConversationArchive *archive)
{
    free(archive);
}

/*
 * Provide the ai conversation archive upsert operation used by this module and its client
 * applications.
 */
/* Checked shared publication refuses stale edits, invalid owners and revision exhaustion without removing the unconditional update contract. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_conversation_archive_upsert(
    UmiAiConversationArchive *archive,
    const UmiAiConversationRecord *record)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (archive == NULL || !record_valid(record)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(archive, record->session.session_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (archive->count >= UMI_AI_CONVERSATION_ARCHIVE_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = archive->count++;
    }
    archive->records[index] = *record;
    ++archive->revision;
    archive->records[index].revision = archive->revision;
    return UMI_STATUS_OK;
}
#endif
/* Compatibility writes and delayed edits share admission and one publication
 * point. Input revision is descriptive; the archive assigns the accepted one. */
static UmiStatus archive_upsert(UmiAiConversationArchive *archive,
    const UmiAiConversationRecord *record, int conditional, uint64_t expected_revision)
{
    size_t index;
    uint64_t next_revision;
    if (archive == NULL || !record_valid(record)) return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiAiConversationArchiveValidate(archive) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    index = find_index(archive, record->session.session_id);
    if (conditional && ((index == SIZE_MAX && expected_revision != 0U) ||
        (index != SIZE_MAX && archive->records[index].revision != expected_revision)))
        return UMI_STATUS_BUSY;
    if (archive->revision == UINT64_MAX ||
        (index == SIZE_MAX && archive->count == UMI_AI_CONVERSATION_ARCHIVE_CAPACITY))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    next_revision = archive->revision + 1U;
    if (index == SIZE_MAX) index = archive->count++;
    archive->records[index] = *record;
    archive->records[index].revision = next_revision;
    archive->revision = next_revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_conversation_archive_upsert(UmiAiConversationArchive *archive,
    const UmiAiConversationRecord *record)
{
    return archive_upsert(archive, record, 0, 0U);
}

UmiStatus UmiAiConversationArchiveUpsertIfRevision(UmiAiConversationArchive *archive,
    const UmiAiConversationRecord *record, uint64_t expected_revision)
{
    return archive_upsert(archive, record, 1, expected_revision);
}

/*
 * Find ai conversation archive while leaving the underlying catalogue or model owned by
 * this module.
 */
/* Validate query text and stored bounds before lookup while preserving caller output on every refusal. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_conversation_archive_find(
    const UmiAiConversationArchive *archive,
    const char *session_id,
    UmiAiConversationRecord *out_record)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (archive == NULL || session_id == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(archive, session_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_record = archive->records[index];
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_ai_conversation_archive_find(
    const UmiAiConversationArchive *archive, const char *session_id,
    UmiAiConversationRecord *out_record)
{
    size_t index;
    if (archive == NULL || !archive_id_valid(session_id) || out_record == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiAiConversationArchiveValidate(archive) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    index = find_index(archive, session_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_record = archive->records[index];
    return UMI_STATUS_OK;
}

/*
 * Find ai conversation archive while leaving the underlying catalogue or model owned by
 * this module.
 */
/* Validate the owner before indexed access so corrupt counts cannot authorize an out-of-range read. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_conversation_archive_at(
    const UmiAiConversationArchive *archive,
    size_t index,
    UmiAiConversationRecord *out_record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (archive == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= archive->count) return UMI_STATUS_NOT_FOUND;
    *out_record = archive->records[index];
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_ai_conversation_archive_at(
    const UmiAiConversationArchive *archive, size_t index,
    UmiAiConversationRecord *out_record)
{
    if (archive == NULL || out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiAiConversationArchiveValidate(archive) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    if (index >= archive->count) return UMI_STATUS_NOT_FOUND;
    *out_record = archive->records[index];
    return UMI_STATUS_OK;
}

/*
 * Remove ai conversation archive while keeping the remaining records in a valid and
 * discoverable state.
 */
/* Conditional removal checks the record identity and revision capacity before shifting or clearing any stored session. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_conversation_archive_remove(
    UmiAiConversationArchive *archive,
    const char *session_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (archive == NULL || session_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(archive, session_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < archive->count) {
        (void)memmove(&archive->records[index],
                      &archive->records[index + 1U],
                      (archive->count - index - 1U) *
                          sizeof(archive->records[0]));
    }
    --archive->count;
    (void)memset(&archive->records[archive->count], 0,
                 sizeof(archive->records[0]));
    ++archive->revision;
    return UMI_STATUS_OK;
}
#endif
static UmiStatus archive_remove(UmiAiConversationArchive *archive,
    const char *session_id, int conditional, uint64_t expected_revision)
{
    size_t index;
    if (archive == NULL || !archive_id_valid(session_id) ||
        (conditional && expected_revision == 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiAiConversationArchiveValidate(archive) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    index = find_index(archive, session_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (conditional && archive->records[index].revision != expected_revision) return UMI_STATUS_BUSY;
    if (archive->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (index + 1U < archive->count)
        (void)memmove(&archive->records[index], &archive->records[index + 1U],
            (archive->count - index - 1U) * sizeof(archive->records[0]));
    --archive->count;
    (void)memset(&archive->records[archive->count], 0, sizeof(archive->records[0]));
    ++archive->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_conversation_archive_remove(UmiAiConversationArchive *archive,
    const char *session_id)
{
    return archive_remove(archive, session_id, 0, 0U);
}

UmiStatus UmiAiConversationArchiveRemoveIfRevision(UmiAiConversationArchive *archive,
    const char *session_id, uint64_t expected_revision)
{
    return archive_remove(archive, session_id, 1, expected_revision);
}

/*
 * Return the number of records represented by ai conversation archive without changing
 * their state.
 */
/* Invalid owners now report no readable records; the explicit validator distinguishes invalid state from a valid empty archive. The former implementation is retained for owner review. */
#if 0
size_t umi_ai_conversation_archive_count(
    const UmiAiConversationArchive *archive)
{
    return archive != NULL ? archive->count : 0U;
}
#endif
size_t umi_ai_conversation_archive_count(const UmiAiConversationArchive *archive)
{
    return UmiAiConversationArchiveValidate(archive) == UMI_STATUS_OK ? archive->count : 0U;
}

/* Provide the builder reserve operation used by this module and its client applications. */
static UmiStatus builder_reserve(TextBuilder *builder, size_t additional)
{
    size_t required;
    size_t capacity;
    char *replacement;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (builder == NULL || additional > SIZE_MAX - builder->length - 1U) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    required = builder->length + additional + 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= builder->capacity) return UMI_STATUS_OK;
    capacity = builder->capacity == 0U ? 4096U : builder->capacity;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    replacement = (char *)realloc(builder->text, capacity);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    builder->text = replacement;
    builder->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the builder appendf operation used by this module and its client applications. */
static UmiStatus builder_appendf(TextBuilder *builder, const char *format, ...)
{
    va_list arguments;
    va_list copy;
    int required;
    UmiStatus status;
    va_start(arguments, format);
    va_copy(copy, arguments);
    required = vsnprintf(NULL, 0U, format, copy);
    va_end(copy);
    /* Apply this branch only when its contract condition is satisfied. */
    if (required < 0) {
        va_end(arguments);
        return UMI_STATUS_INTERNAL_ERROR;
    }
    status = builder_reserve(builder, (size_t)required);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        int written = vsnprintf(builder->text + builder->length,
                                builder->capacity - builder->length,
                                format, arguments);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (written != required) status = UMI_STATUS_INTERNAL_ERROR;
        /* Use this fallback path when the earlier condition does not apply. */
        else builder->length += (size_t)written;
    }
    va_end(arguments);
    return status;
}

/* Provide the escape byte operation used by this module and its client applications. */
static int escape_byte(unsigned char value)
{
    return value < 32U || value > 126U || value == '%' || value == '=';
}

/* Provide the hex digit operation used by this module and its client applications. */
static char hex_digit(unsigned int value)
{
    return (char)(value < 10U ? ('0' + value) : ('A' + value - 10U));
}

/* Provide the encode text operation used by this module and its client applications. */
static UmiStatus encode_text(const char *source, char **out_encoded)
{
    size_t input_length;
    size_t required = 1U;
    size_t input;
    size_t output = 0U;
    char *encoded;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL || out_encoded == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_encoded = NULL;
    input_length = strlen(source);
    /* Visit each bounded item once so every record receives the same rule. */
    for (input = 0U; input < input_length; ++input) {
        size_t width = escape_byte((unsigned char)source[input]) ? 3U : 1U;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (required > SIZE_MAX - width) return UMI_STATUS_CAPACITY_EXCEEDED;
        required += width;
    }
    encoded = (char *)malloc(required);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (encoded == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Visit each bounded item once so every record receives the same rule. */
    for (input = 0U; input < input_length; ++input) {
        unsigned char value = (unsigned char)source[input];
        /* Apply this branch only when its contract condition is satisfied. */
        if (escape_byte(value)) {
            encoded[output++] = '%';
            encoded[output++] = hex_digit((unsigned int)(value >> 4U));
            encoded[output++] = hex_digit((unsigned int)(value & 0x0FU));
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            encoded[output++] = (char)value;
        }
    }
    encoded[output] = '\0';
    *out_encoded = encoded;
    return UMI_STATUS_OK;
}

/* Provide the append field operation used by this module and its client applications. */
static UmiStatus append_field(TextBuilder *builder,
                              const char *key,
                              const char *value)
{
    char *encoded = NULL;
    UmiStatus status = encode_text(value, &encoded);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = builder_appendf(builder, "%s=%s\n", key, encoded);
    }
    free(encoded);
    return status;
}

/*
 * Provide the ai conversation archive save record operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_conversation_archive_save_record(
    const UmiAiConversationRecord *record,
    const UmiAiPrivacyPolicy *privacy,
    const char *path)
{
    TextBuilder builder = {0};
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!record_valid(record) || privacy == NULL || path == NULL ||
        path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ai_privacy_policy_check_persistence(
        privacy, record->classification, record->persistence_approved);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = builder_appendf(&builder, "format=umicom-ai-session-v1\n");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_field(
        &builder, "session.id", record->session.session_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_field(
        &builder, "session.provider", record->session.provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_field(
        &builder, "session.model", record->session.model_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = builder_appendf(
        &builder, "session.created=%llu\n",
        (unsigned long long)record->session.created_at_ns);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_field(
        &builder, "record.workspace", record->workspace_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_field(
        &builder, "record.title", record->title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = builder_appendf(
        &builder,
        "record.classification=%d\nrecord.remote-approved=%d\n"
        "record.persistence-approved=%d\nrecord.updated=%llu\n"
        "record.revision=%llu\nmessage.count=%zu\n",
        (int)record->classification,
        record->remote_approved != 0,
        record->persistence_approved != 0,
        (unsigned long long)record->updated_at_ns,
        (unsigned long long)record->revision,
        record->conversation.message_count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK &&
         index < record->conversation.message_count; ++index) {
        char key[96];
        const UmiAiMessage *message = &record->conversation.messages[index];
        status = builder_appendf(&builder, "message.%zu.role=%d\n",
                                 index, (int)message->role);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            (void)snprintf(key, sizeof(key), "message.%zu.name", index);
            status = append_field(&builder, key, message->name);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            (void)snprintf(key, sizeof(key), "message.%zu.text", index);
            status = append_field(&builder, key, message->text);
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_atomic_file_write_text(path, builder.text);
    }
    free(builder.text);
    return status;
}

/* Provide the hex value operation used by this module and its client applications. */
static int hex_value(char value)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= '0' && value <= '9') return value - '0';
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    return -1;
}

/* Provide the decode text operation used by this module and its client applications. */
/* Escaped zero bytes are now refused so a serialized field cannot silently hide trailing content. The former implementation is retained for owner review. */
#if 0
static UmiStatus decode_text(const char *encoded,
                             char *destination,
                             size_t capacity)
{
    size_t input = 0U;
    size_t output = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (encoded == NULL || destination == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (encoded[input] != '\0') {
        unsigned char value;
        /* Apply this branch only when its contract condition is satisfied. */
        if (encoded[input] == '%') {
            int high;
            int low;
            /* Apply this branch only when its contract condition is satisfied. */
            if (encoded[input + 1U] == '\0' || encoded[input + 2U] == '\0') {
                return UMI_STATUS_PARSE_ERROR;
            }
            high = hex_value(encoded[input + 1U]);
            low = hex_value(encoded[input + 2U]);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (high < 0 || low < 0) return UMI_STATUS_PARSE_ERROR;
            value = (unsigned char)((unsigned int)high * 16U +
                                    (unsigned int)low);
            input += 3U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            value = (unsigned char)encoded[input++];
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (output + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        destination[output++] = (char)value;
    }
    destination[output] = '\0';
    return UMI_STATUS_OK;
}
#endif
static UmiStatus decode_text(const char *encoded,
                             char *destination,
                             size_t capacity)
{
    size_t input = 0U;
    size_t output = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (encoded == NULL || destination == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (encoded[input] != '\0') {
        unsigned char value;
        /* Apply this branch only when its contract condition is satisfied. */
        if (encoded[input] == '%') {
            int high;
            int low;
            /* Apply this branch only when its contract condition is satisfied. */
            if (encoded[input + 1U] == '\0' || encoded[input + 2U] == '\0') {
                return UMI_STATUS_PARSE_ERROR;
            }
            high = hex_value(encoded[input + 1U]);
            low = hex_value(encoded[input + 2U]);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (high < 0 || low < 0) return UMI_STATUS_PARSE_ERROR;
            value = (unsigned char)((unsigned int)high * 16U +
                                    (unsigned int)low);
            input += 3U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            value = (unsigned char)encoded[input++];
        }
        /* A decoded NUL would hide the remaining bytes from C string consumers. */
        if (value == 0U) return UMI_STATUS_PARSE_ERROR;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (output + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        destination[output++] = (char)value;
    }
    destination[output] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the parse u64 operation used by this module and its client applications. */
/* Strict shared unsigned parsing replaces locale-sensitive conversion that admitted signs and whitespace. The former implementation is retained for owner review. */
#if 0
static UmiStatus parse_u64(const char *text, uint64_t *out_value)
{
    char *end = NULL;
    unsigned long long value;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_value == NULL || text[0] == '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }
    errno = 0;
    value = strtoull(text, &end, 10);
    /* Apply this branch only when its contract condition is satisfied. */
    if (errno != 0 || end == text || *end != '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }
    *out_value = (uint64_t)value;
    return UMI_STATUS_OK;
}
#endif
static UmiStatus parse_u64(const char *text, uint64_t *out_value)
{
    if (text == NULL || out_value == NULL) return UMI_STATUS_PARSE_ERROR;
    /* The shared decimal parser admits digits only and checks overflow before
     * multiplication. Translate invalid persisted numbers to the format error. */
    return umi_ascii_uint64_parse(text, strlen(text), out_value) == UMI_STATUS_OK
        ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

/* Provide the parse message key operation used by this module and its client applications. */
/* Message indices now use bounded digit-only parsing before narrowing and indexing fixed storage. The former implementation is retained for owner review. */
#if 0
static UmiStatus parse_message_key(const char *key,
                                   size_t *out_index,
                                   const char **out_field)
{
    const char *cursor;
    char *end = NULL;
    unsigned long value;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (key == NULL || strncmp(key, "message.", 8U) != 0 ||
        out_index == NULL || out_field == NULL) {
        return UMI_STATUS_PARSE_ERROR;
    }
    cursor = key + 8U;
    errno = 0;
    value = strtoul(cursor, &end, 10);
    /* Apply this branch only when its contract condition is satisfied. */
    if (errno != 0 || end == cursor || *end != '.' ||
        value >= UMI_AI_MAX_MESSAGES) {
        return UMI_STATUS_PARSE_ERROR;
    }
    *out_index = (size_t)value;
    *out_field = end + 1;
    return UMI_STATUS_OK;
}
#endif
static UmiStatus parse_message_key(const char *key,
    size_t *out_index, const char **out_field)
{
    const char *digits, *separator;
    uint64_t index;
    if (key == NULL || strncmp(key, "message.", 8U) != 0 ||
        out_index == NULL || out_field == NULL) return UMI_STATUS_PARSE_ERROR;
    digits = key + 8U;
    separator = strchr(digits, '.');
    if (separator == NULL ||
        umi_ascii_uint64_parse(digits, (size_t)(separator - digits), &index) != UMI_STATUS_OK ||
        index >= UMI_AI_MAX_MESSAGES) return UMI_STATUS_PARSE_ERROR;
    *out_index = (size_t)index;
    *out_field = separator + 1U;
    return UMI_STATUS_OK;
}

/* Provide the parse field operation used by this module and its client applications. */
/* Persisted classification is range-checked before enum conversion so oversized values cannot alias valid classifications. The former implementation is retained for owner review. */
#if 0
static UmiStatus parse_field(UmiAiConversationRecord *record,
                             const char *key,
                             const char *value,
                             size_t *message_count)
{
    uint64_t number;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "format") == 0) {
        return strcmp(value, "umicom-ai-session-v1") == 0
            ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "session.id") == 0) return decode_text(
        value, record->session.session_id, sizeof(record->session.session_id));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "session.provider") == 0) return decode_text(
        value, record->session.provider_id, sizeof(record->session.provider_id));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "session.model") == 0) return decode_text(
        value, record->session.model_id, sizeof(record->session.model_id));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "session.created") == 0) return parse_u64(
        value, &record->session.created_at_ns);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "record.workspace") == 0) return decode_text(
        value, record->workspace_id, sizeof(record->workspace_id));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "record.title") == 0) return decode_text(
        value, record->title, sizeof(record->title));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "record.updated") == 0) return parse_u64(
        value, &record->updated_at_ns);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "record.revision") == 0) return parse_u64(
        value, &record->revision);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "record.classification") == 0 ||
        strcmp(key, "record.remote-approved") == 0 ||
        strcmp(key, "record.persistence-approved") == 0 ||
        strcmp(key, "message.count") == 0) {
        UmiStatus status = parse_u64(value, &number);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(key, "record.classification") == 0) {
            record->classification = (UmiAiDataClassification)number;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(key, "record.remote-approved") == 0) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (number > 1U) return UMI_STATUS_PARSE_ERROR;
            record->remote_approved = number != 0U;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(key, "record.persistence-approved") == 0) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (number > 1U) return UMI_STATUS_PARSE_ERROR;
            record->persistence_approved = number != 0U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (number > UMI_AI_MAX_MESSAGES) return UMI_STATUS_PARSE_ERROR;
            *message_count = (size_t)number;
        }
        return UMI_STATUS_OK;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(key, "message.", 8U) == 0) {
        size_t index;
        const char *field;
        UmiStatus status = parse_message_key(key, &index, &field);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(field, "role") == 0) {
            status = parse_u64(value, &number);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK || number < UMI_AI_ROLE_SYSTEM ||
                number > UMI_AI_ROLE_TOOL) {
                return UMI_STATUS_PARSE_ERROR;
            }
            record->conversation.messages[index].role = (UmiAiRole)number;
            return UMI_STATUS_OK;
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(field, "name") == 0) return decode_text(
            value, record->conversation.messages[index].name,
            sizeof(record->conversation.messages[index].name));
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(field, "text") == 0) return decode_text(
            value, record->conversation.messages[index].text,
            sizeof(record->conversation.messages[index].text));
    }
    return UMI_STATUS_PARSE_ERROR;
}
#endif
static UmiStatus parse_field(UmiAiConversationRecord *record,
                             const char *key,
                             const char *value,
                             size_t *message_count)
{
    uint64_t number;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "format") == 0) {
        return strcmp(value, "umicom-ai-session-v1") == 0
            ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "session.id") == 0) return decode_text(
        value, record->session.session_id, sizeof(record->session.session_id));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "session.provider") == 0) return decode_text(
        value, record->session.provider_id, sizeof(record->session.provider_id));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "session.model") == 0) return decode_text(
        value, record->session.model_id, sizeof(record->session.model_id));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "session.created") == 0) return parse_u64(
        value, &record->session.created_at_ns);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "record.workspace") == 0) return decode_text(
        value, record->workspace_id, sizeof(record->workspace_id));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "record.title") == 0) return decode_text(
        value, record->title, sizeof(record->title));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "record.updated") == 0) return parse_u64(
        value, &record->updated_at_ns);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "record.revision") == 0) return parse_u64(
        value, &record->revision);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "record.classification") == 0 ||
        strcmp(key, "record.remote-approved") == 0 ||
        strcmp(key, "record.persistence-approved") == 0 ||
        strcmp(key, "message.count") == 0) {
        UmiStatus status = parse_u64(value, &number);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(key, "record.classification") == 0) {
            /* Check before narrowing: a large integer must not wrap into a less
             * restrictive classification value. */
            if (number < (uint64_t)UMI_AI_DATA_PUBLIC || number > (uint64_t)UMI_AI_DATA_RESTRICTED)
                return UMI_STATUS_PARSE_ERROR;
            record->classification = (UmiAiDataClassification)number;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(key, "record.remote-approved") == 0) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (number > 1U) return UMI_STATUS_PARSE_ERROR;
            record->remote_approved = number != 0U;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(key, "record.persistence-approved") == 0) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (number > 1U) return UMI_STATUS_PARSE_ERROR;
            record->persistence_approved = number != 0U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (number > UMI_AI_MAX_MESSAGES) return UMI_STATUS_PARSE_ERROR;
            *message_count = (size_t)number;
        }
        return UMI_STATUS_OK;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(key, "message.", 8U) == 0) {
        size_t index;
        const char *field;
        UmiStatus status = parse_message_key(key, &index, &field);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(field, "role") == 0) {
            status = parse_u64(value, &number);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK || number < UMI_AI_ROLE_SYSTEM ||
                number > UMI_AI_ROLE_TOOL) {
                return UMI_STATUS_PARSE_ERROR;
            }
            record->conversation.messages[index].role = (UmiAiRole)number;
            return UMI_STATUS_OK;
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(field, "name") == 0) return decode_text(
            value, record->conversation.messages[index].name,
            sizeof(record->conversation.messages[index].name));
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(field, "text") == 0) return decode_text(
            value, record->conversation.messages[index].text,
            sizeof(record->conversation.messages[index].text));
    }
    return UMI_STATUS_PARSE_ERROR;
}

/*
 * Provide the ai conversation archive load record operation used by this module and its
 * client applications.
 */
/* Staged complete-format parsing preserves caller state on failure and rejects duplicate, omitted, hidden or ambiguous fields. The compatibility loader retains path semantics; an additive bounded reader handles explicit untrusted-file limits. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_conversation_archive_load_record(
    const char *path,
    UmiAiConversationRecord *out_record)
{
    char *text = NULL;
    char *cursor;
    size_t message_count = 0U;
    int saw_format = 0;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0' || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_fs_read_text(path, &text, NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_record, 0, sizeof(*out_record));
    cursor = text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (status == UMI_STATUS_OK && *cursor != '\0') {
        char *line = cursor;
        char *newline = strchr(cursor, '\n');
        char *equals;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (newline != NULL) {
            *newline = '\0';
            cursor = newline + 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            cursor += strlen(cursor);
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (line[0] == '\0') continue;
        equals = strchr(line, '=');
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (equals == NULL) {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        *equals = '\0';
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(line, "format") == 0) saw_format = 1;
        status = parse_field(out_record, line, equals + 1U, &message_count);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        out_record->conversation.message_count = message_count;
        (void)copy_text(out_record->conversation.conversation_id,
                        sizeof(out_record->conversation.conversation_id),
                        out_record->session.session_id);
        /* Apply this operation only while the related capability or state is available. */
        if (!saw_format || !record_valid(out_record)) {
            status = UMI_STATUS_PARSE_ERROR;
        }
    }
    umi_fs_free_text(text);
    return status;
}
#endif
/* Format identity is tracked independently of field values. Even an empty
 * optional field must occur exactly once; message records cannot be hidden
 * outside the declared count or repeated through an alternate numeric spelling. */
static UmiStatus parse_unique_field(UmiAiConversationRecord *record,
    const char *key, const char *value, size_t *message_count,
    uint32_t *seen, unsigned char message_seen[UMI_AI_MAX_MESSAGES])
{
    static const char *const keys[] = {
        "format", "session.id", "session.provider", "session.model", "session.created",
        "record.workspace", "record.title", "record.classification",
        "record.remote-approved", "record.persistence-approved", "record.updated",
        "record.revision", "message.count"
    };
    for (size_t index = 0U; index < sizeof keys / sizeof keys[0]; ++index) {
        if (strcmp(key, keys[index]) == 0) {
            uint32_t bit = UINT32_C(1) << index;
            UmiStatus status;
            if ((*seen & bit) != 0U) return UMI_STATUS_PARSE_ERROR;
            status = parse_field(record, key, value, message_count);
            if (status == UMI_STATUS_OK) *seen |= bit;
            return status;
        }
    }
    {
        size_t index;
        const char *field;
        unsigned char bit;
        UmiStatus status = parse_message_key(key, &index, &field);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(field, "role") == 0) bit = 1U;
        else if (strcmp(field, "name") == 0) bit = 2U;
        else if (strcmp(field, "text") == 0) bit = 4U;
        else return UMI_STATUS_PARSE_ERROR;
        if ((message_seen[index] & bit) != 0U) return UMI_STATUS_PARSE_ERROR;
        status = parse_field(record, key, value, message_count);
        if (status == UMI_STATUS_OK) message_seen[index] |= bit;
        return status;
    }
}

UmiStatus UmiAiConversationRecordParse(const void *bytes, size_t size,
    UmiAiConversationRecord *out_record)
{
    UmiAiConversationRecord *candidate;
    char *text, *cursor;
    size_t message_count = 0U;
    uint32_t seen = 0U;
    unsigned char message_seen[UMI_AI_MAX_MESSAGES] = {0};
    UmiStatus status = UMI_STATUS_OK;
    if (bytes == NULL || out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (size > UMI_AI_CONVERSATION_FILE_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (size == 0U || memchr(bytes, '\0', size) != NULL) return UMI_STATUS_PARSE_ERROR;
    /* Stage both input and output before parsing. Input may borrow bytes from
     * the caller's old record; every failure preserves that complete record. */
    text = malloc(size + 1U);
    candidate = calloc(1U, sizeof(*candidate));
    if (text == NULL || candidate == NULL) {
        free(text); free(candidate);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(text, bytes, size); text[size] = '\0';
    cursor = text;
    while (status == UMI_STATUS_OK && *cursor != '\0') {
        char *line = cursor;
        char *newline = strchr(cursor, '\n');
        char *equals;
        size_t line_size;
        if (newline != NULL) { *newline = '\0'; cursor = newline + 1U; }
        else cursor += strlen(cursor);
        line_size = strlen(line);
        if (line_size != 0U && line[line_size - 1U] == '\r') line[--line_size] = '\0';
        if (line_size == 0U) continue;
        equals = strchr(line, '=');
        if (equals == NULL) { status = UMI_STATUS_PARSE_ERROR; break; }
        *equals = '\0';
        status = parse_unique_field(candidate, line, equals + 1U, &message_count, &seen, message_seen);
    }
    if (status == UMI_STATUS_OK && seen != UINT32_C(0x1FFF)) status = UMI_STATUS_PARSE_ERROR;
    for (size_t index = 0U; status == UMI_STATUS_OK && index < UMI_AI_MAX_MESSAGES; ++index)
        if (message_seen[index] != (index < message_count ? 7U : 0U)) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) {
        candidate->conversation.message_count = message_count;
        if (!copy_text(candidate->conversation.conversation_id,
                sizeof candidate->conversation.conversation_id, candidate->session.session_id) ||
            !record_valid(candidate)) status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK) *out_record = *candidate;
    free(candidate); free(text);
    return status;
}

UmiStatus UmiAiConversationArchiveLoadBounded(const char *absolute_path,
    size_t maximum_bytes, UmiAiConversationRecord *out_record)
{
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiStatus status;
    if (absolute_path == NULL || absolute_path[0] == '\0' || out_record == NULL ||
        maximum_bytes == 0U || maximum_bytes > UMI_AI_CONVERSATION_FILE_MAX_BYTES)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* The shared reader refuses oversize before allocation and owns handle
     * consistency/regular-file checks. Its absolute path rule avoids cwd races. */
    status = UmiInputFileRead(absolute_path, maximum_bytes, &bytes, &size);
    if (status == UMI_STATUS_OK) status = UmiAiConversationRecordParse(bytes, size, out_record);
    UmiInputFileFree(bytes);
    return status;
}

UmiStatus umi_ai_conversation_archive_load_record(
    const char *path, UmiAiConversationRecord *out_record)
{
    char *text = NULL;
    size_t size = 0U;
    UmiStatus status;
    if (path == NULL || path[0] == '\0' || out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Retain the compatibility reader's relative-path/link semantics. It still
     * allocates the whole file first; use LoadBounded for untrusted input. Both
     * paths now share strict staged parsing and preserve output on any error. */
    status = umi_fs_read_text(path, &text, &size);
    if (status == UMI_STATUS_OK) status = UmiAiConversationRecordParse(text, size, out_record);
    umi_fs_free_text(text);
    return status;
}
