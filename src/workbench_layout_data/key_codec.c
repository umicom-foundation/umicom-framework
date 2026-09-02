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

/* Provide the record segment operation used by this module and its client applications. */
static const char *record_segment(UmiWorkbenchLayoutDataRecordKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/* Provide the parse segment operation used by this module and its client applications. */
static UmiWorkbenchLayoutDataRecordKind parse_segment(const char *segment)
{
    UmiWorkbenchLayoutDataRecordKind kind;
    /* Visit each bounded item once so every record receives the same rule. */
    for (kind = UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST;
         kind <= UMI_WORKBENCH_LAYOUT_DATA_RECORD_METADATA;
         kind = (UmiWorkbenchLayoutDataRecordKind)((int)kind + 1)) {
        const char *candidate = record_segment(kind);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (candidate != NULL && strcmp(candidate, segment) == 0) {
            return kind;
        }
    }
    return (UmiWorkbenchLayoutDataRecordKind)0;
}

/* Check that segment satisfies its contract before another service relies on it. */
static bool segment_valid(const char *segment)
{
    const unsigned char *cursor = (const unsigned char *)segment;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (segment == NULL || segment[0] == '\0') return false;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
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
        /* Apply this branch only when its contract condition is satisfied. */
        if (!accepted) return false;
        ++cursor;
    }
    return true;
}

/*
 * Provide the workbench layout data key build operation used by this module and its client
 * applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (segment == NULL || buffer == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (aggregate_id != NULL && aggregate_id[0] != '\0' &&
        !segment_valid(aggregate_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record_id != NULL && record_id[0] != '\0' &&
        !segment_valid(record_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK ||
        kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK) {
        written = snprintf(buffer, capacity, "%s/%s/%s/%zu",
                           UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT,
                           segment,
                           aggregate_id != NULL ? aggregate_id : "",
                           chunk_index);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_REVISION ||
               kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_CHANGE ||
               kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_OUTBOX) {
        written = snprintf(buffer, capacity, "%s/%s/%020llu/%s",
                           UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT,
                           segment,
                           (unsigned long long)sequence,
                           record_id != NULL ? record_id : "");
    } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (aggregate_id != NULL && aggregate_id[0] != '\0' &&
               record_id != NULL && record_id[0] != '\0') {
        written = snprintf(buffer, capacity, "%s/%s/%s/%s",
                           UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT,
                           segment, aggregate_id, record_id);
    } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (aggregate_id != NULL && aggregate_id[0] != '\0') {
        written = snprintf(buffer, capacity, "%s/%s/%s",
                           UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT,
                           segment, aggregate_id);
    } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (record_id != NULL && record_id[0] != '\0') {
        written = snprintf(buffer, capacity, "%s/%s/%s",
                           UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT,
                           segment, record_id);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        written = snprintf(buffer, capacity, "%s/%s",
                           UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT,
                           segment);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (written < 0) {
        buffer[0] = '\0';
        return UMI_STATUS_INTERNAL_ERROR;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((size_t)written >= capacity ||
        (size_t)written >= UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY) {
        buffer[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout data key prefix operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_data_key_prefix(
    UmiWorkbenchLayoutDataRecordKind kind,
    const char *aggregate_id,
    char *buffer,
    size_t capacity)
{
    const char *segment = record_segment(kind);
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (segment == NULL || buffer == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (aggregate_id != NULL && aggregate_id[0] != '\0' &&
        !segment_valid(aggregate_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = aggregate_id != NULL && aggregate_id[0] != '\0'
        ? snprintf(buffer, capacity, "%s/%s/%s/",
                   UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT, segment, aggregate_id)
        : snprintf(buffer, capacity, "%s/%s/",
                   UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT, segment);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((size_t)written >= capacity) {
        buffer[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/*
 * Read workbench layout data key into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_data_key_parse(
    const char *key,
    UmiWorkbenchLayoutDataKeyParts *out_parts)
{
    char copy[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char *segments[6];
    char *cursor;
    size_t count = 0U;
    UmiWorkbenchLayoutDataRecordKind kind;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (key == NULL || out_parts == NULL ||
        strlen(key) >= sizeof(copy)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_parts, 0, sizeof(*out_parts));
    out_parts->structure_size = sizeof(*out_parts);
    out_parts->chunk_index = UMI_WORKBENCH_LAYOUT_DATA_INDEX_NONE;
    (void)memcpy(copy, key, strlen(key) + 1U);

    cursor = strtok(copy, "/");
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor != NULL && count < 6U) {
        segments[count++] = cursor;
        cursor = strtok(NULL, "/");
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count < 2U || strcmp(segments[0],
                            UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT) != 0) {
        return UMI_STATUS_PARSE_ERROR;
    }
    kind = parse_segment(segments[1]);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((int)kind == 0) return UMI_STATUS_PARSE_ERROR;
    out_parts->kind = kind;

    /* Apply this branch only when its contract condition is satisfied. */
    if (kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK ||
        kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK) {
        char *end = NULL;
        unsigned long parsed;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (count != 4U) return UMI_STATUS_PARSE_ERROR;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_workbench_layout_data_copy_text(
                out_parts->aggregate_id,
                sizeof(out_parts->aggregate_id),
                segments[2], false) != UMI_STATUS_OK) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        parsed = strtoul(segments[3], &end, 10);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL || *end != '\0') return UMI_STATUS_PARSE_ERROR;
        out_parts->chunk_index = (size_t)parsed;
        return UMI_STATUS_OK;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_REVISION ||
        kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_CHANGE ||
        kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_OUTBOX) {
        char *end = NULL;
        unsigned long long parsed;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (count < 3U || count > 4U) return UMI_STATUS_PARSE_ERROR;
        parsed = strtoull(segments[2], &end, 10);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL || *end != '\0') return UMI_STATUS_PARSE_ERROR;
        out_parts->sequence = (uint64_t)parsed;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (count == 4U) {
            return umi_workbench_layout_data_copy_text(
                out_parts->record_id, sizeof(out_parts->record_id),
                segments[3], true);
        }
        return UMI_STATUS_OK;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count >= 3U) {
        UmiStatus status = umi_workbench_layout_data_copy_text(
            out_parts->aggregate_id, sizeof(out_parts->aggregate_id),
            segments[2], true);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count >= 4U) {
        return umi_workbench_layout_data_copy_text(
            out_parts->record_id, sizeof(out_parts->record_id),
            segments[3], true);
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout data key is owned operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_data_key_is_owned(const char *key)
{
    static const char prefix[] = UMI_WORKBENCH_LAYOUT_DATA_KEY_ROOT "/";
    return key != NULL && strncmp(key, prefix, sizeof(prefix) - 1U) == 0;
}

/*
 * Provide the workbench layout data key matches prefix operation used by this module and
 * its client applications.
 */
bool umi_workbench_layout_data_key_matches_prefix(
    const char *key,
    const char *prefix)
{
    return umi_workbench_layout_data_text_has_prefix(key, prefix);
}
