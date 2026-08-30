/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/key_codec.c
 *
 * PURPOSE:
 *   Implement deterministic, bounded and parseable key construction for the authoritative layout Data Server namespace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/key_codec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT "workbench-layout"

static const char *record_segment(UmiWorkbenchLayoutDataRecordKind kind)
{
    switch (kind) {
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST:
        return "layout-manifest";
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK:
        return "layout-chunk";
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_MANIFEST:
        return "session-manifest";
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK:
        return "session-chunk";
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_REVISION:
        return "revision";
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_GRANT:
        return "grant";
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_LEASE:
        return "lease";
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_CHANGE:
        return "change";
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_OUTBOX:
        return "outbox";
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_CONFLICT:
        return "conflict";
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_SYNC_CURSOR:
        return "sync-cursor";
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_OFFLINE_OPERATION:
        return "offline-operation";
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_PRESENCE:
        return "presence";
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_MIGRATION:
        return "migration";
    case UMI_WORKBENCH_LAYOUT_DATA_RECORD_METADATA:
        return "metadata";
    default:
        return NULL;
    }
}

static UmiWorkbenchLayoutDataRecordKind parse_segment(const char *segment)
{
    UmiWorkbenchLayoutDataRecordKind kind;
    for (kind = UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST;
         kind <= UMI_WORKBENCH_LAYOUT_DATA_RECORD_METADATA;
         kind = (UmiWorkbenchLayoutDataRecordKind)((int)kind + 1)) {
        const char *candidate = record_segment(kind);
        if (candidate != NULL && strcmp(candidate, segment) == 0) {
            return kind;
        }
    }
    return (UmiWorkbenchLayoutDataRecordKind)0;
}

static bool segment_valid(const char *segment)
{
    const unsigned char *cursor = (const unsigned char *)segment;
    if (segment == NULL || segment[0] == '\0') return false;
    while (*cursor != 0U) {
        const unsigned char character = *cursor;
        const bool accepted =
            (character >= (unsigned char)'a' &&
             character <= (unsigned char)'z') ||
            (character >= (unsigned char)'A' &&
             character <= (unsigned char)'Z') ||
            (character >= (unsigned char)'0' &&
             character <= (unsigned char)'9') ||
            character == (unsigned char)'-' ||
            character == (unsigned char)'_' ||
            character == (unsigned char)'.' ||
            character == (unsigned char)'@';
        if (!accepted) return false;
        ++cursor;
    }
    return true;
}

UmiStatus umi_workbench_layout_data_key_build(
    UmiWorkbenchLayoutDataRecordKind kind,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    size_t chunk_index,
    char *buffer,
    size_t capacity)
{
    const char *segment = record_segment(kind);
    int written;
    if (segment == NULL || buffer == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (aggregate_id != NULL && aggregate_id[0] != '\0' &&
        !segment_valid(aggregate_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (record_id != NULL && record_id[0] != '\0' &&
        !segment_valid(record_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK ||
        kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK) {
        written = snprintf(buffer, capacity, "%s/%s/%s/%zu",
                           UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT,
                           segment,
                           aggregate_id != NULL ? aggregate_id : "",
                           chunk_index);
    } else if (kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_REVISION ||
               kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_CHANGE ||
               kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_OUTBOX) {
        written = snprintf(buffer, capacity, "%s/%s/%020llu/%s",
                           UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT,
                           segment,
                           (unsigned long long)sequence,
                           record_id != NULL ? record_id : "");
    } else if (aggregate_id != NULL && aggregate_id[0] != '\0' &&
               record_id != NULL && record_id[0] != '\0') {
        written = snprintf(buffer, capacity, "%s/%s/%s/%s",
                           UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT,
                           segment, aggregate_id, record_id);
    } else if (aggregate_id != NULL && aggregate_id[0] != '\0') {
        written = snprintf(buffer, capacity, "%s/%s/%s",
                           UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT,
                           segment, aggregate_id);
    } else if (record_id != NULL && record_id[0] != '\0') {
        written = snprintf(buffer, capacity, "%s/%s/%s",
                           UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT,
                           segment, record_id);
    } else {
        written = snprintf(buffer, capacity, "%s/%s",
                           UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT,
                           segment);
    }
    if (written < 0) {
        buffer[0] = '\0';
        return UMI_STATUS_INTERNAL_ERROR;
    }
    if ((size_t)written >= capacity ||
        (size_t)written >= UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY) {
        buffer[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_data_key_prefix(
    UmiWorkbenchLayoutDataRecordKind kind,
    const char *aggregate_id,
    char *buffer,
    size_t capacity)
{
    const char *segment = record_segment(kind);
    int written;
    if (segment == NULL || buffer == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (aggregate_id != NULL && aggregate_id[0] != '\0' &&
        !segment_valid(aggregate_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = aggregate_id != NULL && aggregate_id[0] != '\0'
        ? snprintf(buffer, capacity, "%s/%s/%s/",
                   UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT, segment, aggregate_id)
        : snprintf(buffer, capacity, "%s/%s/",
                   UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT, segment);
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    if ((size_t)written >= capacity) {
        buffer[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_data_key_parse(
    const char *key,
    UmiWorkbenchLayoutDataKeyParts *out_parts)
{
    char copy[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char *segments[6];
    char *cursor;
    size_t count = 0U;
    UmiWorkbenchLayoutDataRecordKind kind;
    if (key == NULL || out_parts == NULL ||
        strlen(key) >= sizeof(copy)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_parts, 0, sizeof(*out_parts));
    out_parts->structure_size = sizeof(*out_parts);
    out_parts->chunk_index = UMI_WORKBENCH_LAYOUT_DATA_INDEX_NONE;
    (void)memcpy(copy, key, strlen(key) + 1U);

    cursor = strtok(copy, "/");
    while (cursor != NULL && count < 6U) {
        segments[count++] = cursor;
        cursor = strtok(NULL, "/");
    }
    if (count < 2U || strcmp(segments[0],
                            UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT) != 0) {
        return UMI_STATUS_PARSE_ERROR;
    }
    kind = parse_segment(segments[1]);
    if ((int)kind == 0) return UMI_STATUS_PARSE_ERROR;
    out_parts->kind = kind;

    if (kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK ||
        kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK) {
        char *end = NULL;
        unsigned long parsed;
        if (count != 4U) return UMI_STATUS_PARSE_ERROR;
        if (umi_workbench_layout_data_copy_text(
                out_parts->aggregate_id,
                sizeof(out_parts->aggregate_id),
                segments[2], false) != UMI_STATUS_OK) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        parsed = strtoul(segments[3], &end, 10);
        if (end == NULL || *end != '\0') return UMI_STATUS_PARSE_ERROR;
        out_parts->chunk_index = (size_t)parsed;
        return UMI_STATUS_OK;
    }

    if (kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_REVISION ||
        kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_CHANGE ||
        kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_OUTBOX) {
        char *end = NULL;
        unsigned long long parsed;
        if (count < 3U || count > 4U) return UMI_STATUS_PARSE_ERROR;
        parsed = strtoull(segments[2], &end, 10);
        if (end == NULL || *end != '\0') return UMI_STATUS_PARSE_ERROR;
        out_parts->sequence = (uint64_t)parsed;
        if (count == 4U) {
            return umi_workbench_layout_data_copy_text(
                out_parts->record_id, sizeof(out_parts->record_id),
                segments[3], true);
        }
        return UMI_STATUS_OK;
    }

    if (count >= 3U) {
        UmiStatus status = umi_workbench_layout_data_copy_text(
            out_parts->aggregate_id, sizeof(out_parts->aggregate_id),
            segments[2], true);
        if (status != UMI_STATUS_OK) return status;
    }
    if (count >= 4U) {
        return umi_workbench_layout_data_copy_text(
            out_parts->record_id, sizeof(out_parts->record_id),
            segments[3], true);
    }
    return UMI_STATUS_OK;
}

bool umi_workbench_layout_data_key_is_owned(const char *key)
{
    static const char prefix[] = UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT "/";
    return key != NULL && strncmp(key, prefix, sizeof(prefix) - 1U) == 0;
}

bool umi_workbench_layout_data_key_matches_prefix(
    const char *key,
    const char *prefix)
{
    return umi_workbench_layout_data_text_has_prefix(key, prefix);
}
