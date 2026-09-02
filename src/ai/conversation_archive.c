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

/*
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

/*
 * Find ai conversation archive while leaving the underlying catalogue or model owned by
 * this module.
 */
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

/*
 * Find ai conversation archive while leaving the underlying catalogue or model owned by
 * this module.
 */
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

/*
 * Remove ai conversation archive while keeping the remaining records in a valid and
 * discoverable state.
 */
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

/*
 * Return the number of records represented by ai conversation archive without changing
 * their state.
 */
size_t umi_ai_conversation_archive_count(
    const UmiAiConversationArchive *archive)
{
    return archive != NULL ? archive->count : 0U;
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

/* Provide the parse u64 operation used by this module and its client applications. */
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

/* Provide the parse message key operation used by this module and its client applications. */
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

/* Provide the parse field operation used by this module and its client applications. */
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

/*
 * Provide the ai conversation archive load record operation used by this module and its
 * client applications.
 */
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
