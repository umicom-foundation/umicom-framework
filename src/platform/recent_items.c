/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/recent_items.c
 *
 * PURPOSE:
 *   Implement a cross-product most-recently-used resource catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Each operation is deliberately small and deterministic. Snapshots are copied
 * into bounded storage, revisions advance on mutation, and callers retain
 * responsibility for higher-level threading and persistence policy.
 */
#include "umicom/platform/recent_items.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/atomic_file.h"
#include "umicom/platform/filesystem.h"

#define UMI_RECENT_ITEMS_FILE_MARKER "UMICOM_RECENT_ITEMS|1"
#define UMI_RECENT_ITEMS_FILE_HEADER UMI_RECENT_ITEMS_FILE_MARKER "\n"
#define UMI_RECENT_ITEMS_FILE_LIMIT (4U * 1024U * 1024U)

struct UmiRecentItemRegistry {
    UmiRecentItemSnapshot items[UMI_PLATFORM_RECENT_ITEMS_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Derive the shared lookup key used when several frontends record one URI. */
UmiStatus umi_platform_recent_item_id_from_uri(
    const char *scope,
    const char *uri,
    char *out_id,
    size_t out_capacity)
{
    uint64_t hash = UINT64_C(14695981039346656037);
    size_t index;
    int written;

    if (scope == NULL || scope[0] == '\0' || uri == NULL ||
        uri[0] == '\0' || out_id == NULL || out_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* FNV-1a gives every frontend the same compact identity for the same URI
     * without storing a second full path in lookup keys. */
    for (index = 0U; uri[index] != '\0'; ++index) {
        hash ^= (uint64_t)(unsigned char)uri[index];
        hash *= UINT64_C(1099511628211);
    }
    written = snprintf(out_id, out_capacity, "%s-%016llx",
                       scope, (unsigned long long)hash);
    return written < 0 || (size_t)written >= out_capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

/* Confirm that a fixed-size public text field contains a terminator before
 * any string function reads it. This rejects malformed snapshots instead of
 * allowing a comparison to continue beyond the structure boundary. */
static int bounded_text_valid(const char *text, size_t capacity, int allow_empty)
{
    if (text == NULL || capacity == 0U ||
        memchr(text, '\0', capacity) == NULL) {
        return 0;
    }
    return allow_empty || text[0] != '\0';
}

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiRecentItemRegistry *registry, const char *id)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

/*
 * Initialise platform recent items registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_platform_recent_items_registry_create(UmiRecentItemRegistry **out_registry)
{
    UmiRecentItemRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiRecentItemRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by platform recent items registry so the same storage can be
 * reused safely.
 */
void umi_platform_recent_items_registry_destroy(UmiRecentItemRegistry *registry)
{
    free(registry);
}

/*
 * Provide the platform recent items registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_platform_recent_items_registry_upsert(UmiRecentItemRegistry *registry, const UmiRecentItemSnapshot *item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || item == NULL ||
        !bounded_text_valid(item->id, sizeof(item->id), 0) ||
        !bounded_text_valid(item->uri, sizeof(item->uri), 1) ||
        !bounded_text_valid(item->label, sizeof(item->label), 1) ||
        !bounded_text_valid(item->kind, sizeof(item->kind), 1)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, item->id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->count >= UMI_PLATFORM_RECENT_ITEMS_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiRecentItemSnapshot);
    registry->items[index].api_version = 1U;
    registry->items[index].pinned = item->pinned != 0;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;

    return UMI_STATUS_OK;
}

/*
 * Remove platform recent items registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_platform_recent_items_registry_remove(UmiRecentItemRegistry *registry, const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < registry->count) {
        memmove(&registry->items[index], &registry->items[index + 1U],
                (registry->count - index - 1U) * sizeof(registry->items[0]));
    }
    registry->count -= 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find platform recent items registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_platform_recent_items_registry_find(const UmiRecentItemRegistry *registry, const char *id, UmiRecentItemSnapshot *out_item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find platform recent items registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_platform_recent_items_registry_at(const UmiRecentItemRegistry *registry, size_t index, UmiRecentItemSnapshot *out_item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the platform recent items registry touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_platform_recent_items_registry_touch(UmiRecentItemRegistry *registry,
                                                     const char *id,
                                                     uint64_t opened_at)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    registry->items[index].last_opened = opened_at;
    /* A saturated counter stays meaningful and never wraps back to zero. */
    if (registry->items[index].open_count < UINT64_MAX) {
        registry->items[index].open_count += 1U;
    }
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the platform recent items registry set pinned operation used by this module and
 * its client applications.
 */
UmiStatus umi_platform_recent_items_registry_set_pinned(UmiRecentItemRegistry *registry,
                                                        const char *id,
                                                        int pinned)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    registry->items[index].pinned = pinned != 0;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/* Enforce a normal history limit without discarding explicitly pinned work. */
UmiStatus umi_platform_recent_items_registry_trim(
    UmiRecentItemRegistry *registry,
    size_t maximum_count)
{
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    while (registry->count > maximum_count) {
        size_t index;
        size_t oldest_index = SIZE_MAX;

        /* Pinned records express a user decision and are never evicted by an
         * automatic capacity policy. */
        for (index = 0U; index < registry->count; ++index) {
            if (registry->items[index].pinned != 0) continue;
            if (oldest_index == SIZE_MAX ||
                registry->items[index].last_opened <
                    registry->items[oldest_index].last_opened) {
                oldest_index = index;
            }
        }
        if (oldest_index == SIZE_MAX) break;
        (void)umi_platform_recent_items_registry_remove(
            registry, registry->items[oldest_index].id);
    }
    return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by platform recent items registry without
 * changing their state.
 */
size_t umi_platform_recent_items_registry_count(const UmiRecentItemRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the platform recent items registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_platform_recent_items_registry_revision(const UmiRecentItemRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

/* Compare ASCII text without changing the process locale. Resource names and
 * paths can still contain UTF-8; non-ASCII bytes are compared exactly. */
static int ascii_equal_folded(unsigned char left, unsigned char right)
{
    if (left >= (unsigned char)'A' && left <= (unsigned char)'Z') {
        left = (unsigned char)(left - (unsigned char)'A' + (unsigned char)'a');
    }
    if (right >= (unsigned char)'A' && right <= (unsigned char)'Z') {
        right = (unsigned char)(right - (unsigned char)'A' + (unsigned char)'a');
    }
    return left == right;
}

/* Find a case-insensitive search fragment inside one bounded snapshot field. */
static int text_contains(const char *text, const char *fragment)
{
    size_t text_index;
    size_t fragment_index;

    if (fragment == NULL || fragment[0] == '\0') return 1;
    if (text == NULL || text[0] == '\0') return 0;

    for (text_index = 0U; text[text_index] != '\0'; ++text_index) {
        for (fragment_index = 0U;
             fragment[fragment_index] != '\0' &&
             text[text_index + fragment_index] != '\0' &&
             ascii_equal_folded(
                 (unsigned char)text[text_index + fragment_index],
                 (unsigned char)fragment[fragment_index]);
             ++fragment_index) {
            /* The loop condition performs the bounded character comparison. */
        }
        if (fragment[fragment_index] == '\0') return 1;
    }
    return 0;
}

/* Keep ordering deterministic so every application presents the same recent
 * work: pinned first, then newest, most-used and finally stable text keys. */
static int item_precedes(const UmiRecentItemSnapshot *left,
                         const UmiRecentItemSnapshot *right)
{
    int comparison;

    if (left->pinned != right->pinned) return left->pinned > right->pinned;
    if (left->last_opened != right->last_opened) {
        return left->last_opened > right->last_opened;
    }
    if (left->open_count != right->open_count) {
        return left->open_count > right->open_count;
    }
    comparison = strcmp(left->label, right->label);
    if (comparison != 0) return comparison < 0;
    return strcmp(left->id, right->id) < 0;
}

/* Check query filters without exposing the registry's internal array. */
static int item_matches(const UmiRecentItemSnapshot *item,
                        const UmiRecentItemQuery *query)
{
    if (query == NULL) return 1;
    if (query->pinned_only != 0 && item->pinned == 0) return 0;
    if (query->kind != NULL && query->kind[0] != '\0' &&
        !text_contains(item->kind, query->kind)) {
        return 0;
    }
    if (query->text == NULL || query->text[0] == '\0') return 1;
    return text_contains(item->label, query->text) ||
           text_contains(item->uri, query->text) ||
           text_contains(item->id, query->text);
}

/* Produce a ranked copy so user interfaces never depend on storage order. */
UmiStatus umi_platform_recent_items_registry_query(
    const UmiRecentItemRegistry *registry,
    const UmiRecentItemQuery *query,
    UmiRecentItemSnapshot *out_items,
    size_t out_capacity,
    size_t *out_count)
{
    size_t result_limit;
    size_t result_count = 0U;
    size_t source_index;

    if (registry == NULL || out_items == NULL || out_capacity == 0U ||
        out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_count = 0U;
    result_limit = query != NULL && query->limit > 0U
        ? query->limit
        : out_capacity;
    if (result_limit > out_capacity) result_limit = out_capacity;

    /* Insert each match into the bounded result set. When the set is full,
     * lower-ranked records are ignored rather than allocating hidden memory. */
    for (source_index = 0U; source_index < registry->count; ++source_index) {
        const UmiRecentItemSnapshot *candidate =
            &registry->items[source_index];
        size_t insert_at = 0U;
        size_t move_index;

        if (!item_matches(candidate, query)) continue;
        while (insert_at < result_count &&
               !item_precedes(candidate, &out_items[insert_at])) {
            insert_at += 1U;
        }
        if (insert_at >= result_limit) continue;
        if (result_count < result_limit) result_count += 1U;
        for (move_index = result_count - 1U;
             move_index > insert_at;
             --move_index) {
            out_items[move_index] = out_items[move_index - 1U];
        }
        out_items[insert_at] = *candidate;
    }

    *out_count = result_count;
    return UMI_STATUS_OK;
}

/* Add one size only after checking for arithmetic overflow. Persistence uses
 * this before allocating so a corrupt or unusually large record cannot wrap
 * the requested buffer size. */
static UmiStatus checked_size_add(size_t *total, size_t amount)
{
    if (total == NULL || *total > SIZE_MAX - amount) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    *total += amount;
    return UMI_STATUS_OK;
}

/* Encode text as hexadecimal bytes. The stable encoding preserves UTF-8 and
 * Windows paths while ensuring that separators and newlines stay structural. */
static void append_hex(char *output, size_t *offset, const char *text)
{
    static const char HEX[] = "0123456789ABCDEF";

    while (*text != '\0') {
        unsigned char value = (unsigned char)*text;
        output[(*offset)++] = HEX[value >> 4U];
        output[(*offset)++] = HEX[value & 0x0FU];
        text += 1;
    }
}

/* Serialize the complete registry and replace the destination atomically. */
UmiStatus umi_platform_recent_items_registry_save(
    const UmiRecentItemRegistry *registry,
    const char *path)
{
    size_t required = strlen(UMI_RECENT_ITEMS_FILE_HEADER) + 1U;
    size_t index;
    size_t offset;
    char *output;
    UmiStatus status;

    if (registry == NULL || path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Hex encoding uses two output characters per input byte. A fixed amount
     * is reserved for identifiers, counters, separators and the line break. */
    for (index = 0U; index < registry->count; ++index) {
        const UmiRecentItemSnapshot *item = &registry->items[index];
        size_t text_bytes = strlen(item->id) + strlen(item->uri) +
            strlen(item->label) + strlen(item->kind);

        if (text_bytes > (SIZE_MAX - 96U) / 2U) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = checked_size_add(&required, text_bytes * 2U + 96U);
        if (status != UMI_STATUS_OK || required > UMI_RECENT_ITEMS_FILE_LIMIT) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }

    output = (char *)malloc(required);
    if (output == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    offset = strlen(UMI_RECENT_ITEMS_FILE_HEADER);
    (void)memcpy(output, UMI_RECENT_ITEMS_FILE_HEADER, offset);

    for (index = 0U; index < registry->count; ++index) {
        const UmiRecentItemSnapshot *item = &registry->items[index];
        int written;

        output[offset++] = 'R';
        output[offset++] = '|';
        append_hex(output, &offset, item->id);
        output[offset++] = '|';
        append_hex(output, &offset, item->uri);
        output[offset++] = '|';
        append_hex(output, &offset, item->label);
        output[offset++] = '|';
        append_hex(output, &offset, item->kind);
        written = snprintf(
            output + offset,
            required - offset,
            "|%llu|%llu|%d\n",
            (unsigned long long)item->last_opened,
            (unsigned long long)item->open_count,
            item->pinned != 0);
        if (written < 0 || (size_t)written >= required - offset) {
            free(output);
            return UMI_STATUS_INTERNAL_ERROR;
        }
        offset += (size_t)written;
    }

    status = umi_atomic_file_write(path, output, offset);
    free(output);
    return status;
}

/* Translate one hexadecimal character without accepting locale-dependent
 * digits. Returning -1 makes malformed persistence data fail closed. */
static int hex_value(char value)
{
    if (value >= '0' && value <= '9') return value - '0';
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    return -1;
}

/* Decode one field into fixed caller-owned storage and always terminate it. */
static UmiStatus decode_hex(const char *encoded,
                            char *output,
                            size_t output_capacity)
{
    size_t encoded_length;
    size_t input_index;
    size_t output_index = 0U;

    if (encoded == NULL || output == NULL || output_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    encoded_length = strlen(encoded);
    if ((encoded_length % 2U) != 0U ||
        encoded_length / 2U >= output_capacity) {
        return UMI_STATUS_PARSE_ERROR;
    }

    for (input_index = 0U; input_index < encoded_length; input_index += 2U) {
        int high = hex_value(encoded[input_index]);
        int low = hex_value(encoded[input_index + 1U]);
        if (high < 0 || low < 0 || (high == 0 && low == 0)) {
            return UMI_STATUS_PARSE_ERROR;
        }
        output[output_index++] = (char)((high << 4) | low);
    }
    output[output_index] = '\0';
    return UMI_STATUS_OK;
}

/* Parse an unsigned counter only when the complete field is numeric. */
static UmiStatus parse_uint64(const char *text, uint64_t *out_value)
{
    char *end = NULL;
    unsigned long long parsed;
    size_t index;

    if (text == NULL || text[0] == '\0' || out_value == NULL) {
        return UMI_STATUS_PARSE_ERROR;
    }
    for (index = 0U; text[index] != '\0'; ++index) {
        if (text[index] < '0' || text[index] > '9') {
            return UMI_STATUS_PARSE_ERROR;
        }
    }
    errno = 0;
    parsed = strtoull(text, &end, 10);
    if (errno == ERANGE || end == NULL || *end != '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }
    *out_value = (uint64_t)parsed;
    return UMI_STATUS_OK;
}

/* Parse and validate one record before it is inserted into the temporary
 * registry. The live result is published only after every line succeeds. */
static UmiStatus parse_record(UmiRecentItemRegistry *registry, char *line)
{
    char *fields[8] = {0};
    size_t field_count = 0U;
    char *cursor = line;
    UmiRecentItemSnapshot item = {0};
    UmiRecentItemSnapshot duplicate = {0};
    uint64_t pinned = 0U;
    UmiStatus status;

    while (field_count < 8U) {
        char *separator;
        fields[field_count++] = cursor;
        separator = strchr(cursor, '|');
        if (separator == NULL) break;
        if (field_count == 8U) return UMI_STATUS_PARSE_ERROR;
        *separator = '\0';
        cursor = separator + 1;
    }
    if (field_count != 8U || strcmp(fields[0], "R") != 0) {
        return UMI_STATUS_PARSE_ERROR;
    }

    status = decode_hex(fields[1], item.id, sizeof(item.id));
    if (status == UMI_STATUS_OK) {
        status = decode_hex(fields[2], item.uri, sizeof(item.uri));
    }
    if (status == UMI_STATUS_OK) {
        status = decode_hex(fields[3], item.label, sizeof(item.label));
    }
    if (status == UMI_STATUS_OK) {
        status = decode_hex(fields[4], item.kind, sizeof(item.kind));
    }
    if (status == UMI_STATUS_OK) {
        status = parse_uint64(fields[5], &item.last_opened);
    }
    if (status == UMI_STATUS_OK) {
        status = parse_uint64(fields[6], &item.open_count);
    }
    if (status == UMI_STATUS_OK) status = parse_uint64(fields[7], &pinned);
    if (status != UMI_STATUS_OK || pinned > 1U || item.id[0] == '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }

    /* Duplicate identifiers indicate damaged or hand-edited state. Replacing
     * the earlier record silently would hide that problem from the caller. */
    if (umi_platform_recent_items_registry_find(
            registry, item.id, &duplicate) == UMI_STATUS_OK) {
        return UMI_STATUS_PARSE_ERROR;
    }
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = 1U;
    item.pinned = pinned != 0U;
    return umi_platform_recent_items_registry_upsert(registry, &item);
}

/* Build a temporary validated registry before returning it to the caller. */
UmiStatus umi_platform_recent_items_registry_load(
    const char *path,
    UmiRecentItemRegistry **out_registry,
    int *out_loaded)
{
    UmiRecentItemRegistry *registry = NULL;
    char *text = NULL;
    char *cursor;
    size_t text_size = 0U;
    size_t marker_length = strlen(UMI_RECENT_ITEMS_FILE_MARKER);
    UmiStatus status;

    if (path == NULL || path[0] == '\0' || out_registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_registry = NULL;
    if (out_loaded != NULL) *out_loaded = 0;

    status = umi_platform_recent_items_registry_create(&registry);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_fs_is_file(path)) {
        *out_registry = registry;
        return UMI_STATUS_OK;
    }

    status = umi_fs_read_text(path, &text, &text_size);
    if (status != UMI_STATUS_OK) {
        umi_platform_recent_items_registry_destroy(registry);
        return status;
    }
    if (text_size > UMI_RECENT_ITEMS_FILE_LIMIT || text_size <= marker_length ||
        memcmp(text, UMI_RECENT_ITEMS_FILE_MARKER, marker_length) != 0) {
        status = UMI_STATUS_PARSE_ERROR;
        goto load_failed;
    }

    cursor = text + marker_length;
    /* Accept files whose line endings were converted by a Windows editor,
     * while still requiring the complete marker to occupy its own line. */
    if (*cursor == '\r') cursor += 1;
    if (*cursor != '\n') {
        status = UMI_STATUS_PARSE_ERROR;
        goto load_failed;
    }
    cursor += 1;
    while (*cursor != '\0') {
        char *line = cursor;
        char *newline = strchr(cursor, '\n');
        size_t line_length;

        if (newline != NULL) {
            *newline = '\0';
            cursor = newline + 1;
        } else {
            cursor += strlen(cursor);
        }
        line_length = strlen(line);
        if (line_length > 0U && line[line_length - 1U] == '\r') {
            line[line_length - 1U] = '\0';
        }
        if (line[0] == '\0') continue;
        status = parse_record(registry, line);
        if (status != UMI_STATUS_OK) goto load_failed;
    }

    umi_fs_free_text(text);
    if (out_loaded != NULL) *out_loaded = 1;
    *out_registry = registry;
    return UMI_STATUS_OK;

load_failed:
    umi_fs_free_text(text);
    umi_platform_recent_items_registry_destroy(registry);
    return status;
}
