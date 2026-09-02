/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/projection.c
 *
 * PURPOSE:
 *   Implement safe canonical-editor to intelligence-workbench conversions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/projection.h"

#include <limits.h>
#include <string.h>

/* Provide the clamp position operation used by this module and its client applications. */
static uint32_t clamp_position(uint64_t value)
{
    return value > (uint64_t)UINT32_MAX ? UINT32_MAX : (uint32_t)value;
}

/* Provide the copy bounded operation used by this module and its client applications. */
static int copy_bounded(char *destination, size_t capacity, const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
    return source[length] != '\0';
}

/*
 * Provide the editor intel projection entry from source operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_projection_entry_from_source(
    UmiEditorIntelEntry *entry,
    const char *id,
    const char *label,
    const char *detail,
    const UmiEditorSourceLocation *source,
    uint32_t score,
    UmiEditorIntelProjectionFlags flags,
    uint64_t revision)
{
    UmiEditorIntelRange range;
    int truncated = 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL || !umi_editor_intel_id_valid(id) ||
        label == NULL || label[0] == '\0' || source == NULL ||
        umi_editor_source_location_validate(source) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    range.start.line = clamp_position(source->line);
    range.start.column = clamp_position(source->column);
    range.end.line = clamp_position(source->end_line);
    range.end.column = clamp_position(source->end_column);
    memset(entry, 0, sizeof(*entry));
    (void)copy_bounded(entry->id, sizeof(entry->id), id);
    truncated |= copy_bounded(entry->label, sizeof(entry->label), label);
    truncated |= copy_bounded(entry->detail, sizeof(entry->detail), detail);
    truncated |= copy_bounded(
        entry->location.path, sizeof(entry->location.path), source->uri);
    entry->location.range = umi_editor_intel_range_normalize(range);
    entry->score = score;
    entry->flags = flags |
        (truncated != 0 ? UMI_EDITOR_INTEL_PROJECTION_TRUNCATED : 0U);
    entry->revision = revision == 0U ? 1U : revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor intel projection entry without source operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_projection_entry_without_source(
    UmiEditorIntelEntry *entry,
    const char *id,
    const char *label,
    const char *detail,
    uint32_t score,
    UmiEditorIntelProjectionFlags flags,
    uint64_t revision)
{
    const UmiEditorIntelRange empty_range = {{0U, 0U}, {0U, 0U}};
    UmiStatus status;
    UmiEditorSourceLocation source;

    status = umi_editor_source_location_initialize(
        &source, "workspace://editor-intelligence", 0U, 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    source.end_line = empty_range.end.line;
    source.end_column = empty_range.end.column;
    return umi_editor_intel_projection_entry_from_source(
        entry, id, label, detail, &source, score, flags, revision);
}

/*
 * Provide the editor intel projection hash text operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_intel_projection_hash_text(const char *text)
{
    const uint64_t offset_basis = UINT64_C(1469598103934665603);
    const uint64_t prime = UINT64_C(1099511628211);
    uint64_t value = offset_basis;
    const unsigned char *cursor = (const unsigned char *)text;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) {
        value ^= (uint64_t)*cursor;
        value *= prime;
        ++cursor;
    }
    return value;
}

/*
 * Provide the editor intel projection score from rank operation used by this module and
 * its client applications.
 */
uint32_t umi_editor_intel_projection_score_from_rank(int32_t rank)
{
    return rank <= 0 ? 0U : (uint32_t)rank;
}
