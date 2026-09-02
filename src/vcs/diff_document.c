/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/diff_document.c
 *
 * PURPOSE:
 *   Implement the diff document behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework unified-diff document. Created by Sammy Hegab, Umicom Foundation. MIT. */
#include "umicom/vcs/diff_document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiVcsDiffDocument {
    char path[UMI_VCS_PATH_CAPACITY];
    UmiVcsDiffLine *lines;
    size_t count;
    size_t capacity;
    size_t additions;
    size_t deletions;
    size_t hunks;
    uint64_t revision;
    int staged;
    int binary;
};
/* Provide the parse hunk header operation used by this module and its client applications. */
static void parse_hunk_header(const char *text, size_t *old_line, size_t *new_line)
{
    const char *old_start = strstr(text, "@@ -");
    const char *new_start;
    unsigned long long old_value;
    unsigned long long new_value;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (old_start == NULL) return;
    new_start = strstr(old_start + 4, " +");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (new_start == NULL) return;
    old_value = strtoull(old_start + 4, NULL, 10);
    new_value = strtoull(new_start + 2, NULL, 10);
    *old_line = (size_t)old_value;
    *new_line = (size_t)new_value;
}

/*
 * Provide the ensure line capacity operation used by this module and its client
 * applications.
 */
static UmiStatus ensure_line_capacity(UmiVcsDiffDocument *document)
{
    UmiVcsDiffLine *resized;
    size_t next_capacity;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (document->count < document->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (document->capacity >= UMI_VCS_MAX_DIFF_LINES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    next_capacity = document->capacity == 0U ? 64U : document->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (next_capacity > UMI_VCS_MAX_DIFF_LINES) {
        next_capacity = UMI_VCS_MAX_DIFF_LINES;
    }
    resized = realloc(document->lines,
                      next_capacity * sizeof(document->lines[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resized == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    document->lines = resized;
    document->capacity = next_capacity;
    return UMI_STATUS_OK;
}
/*
 * Initialise vcs diff document from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_vcs_diff_document_create(UmiVcsDiffDocument **out_document)
{
    UmiVcsDiffDocument *document;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_document = NULL; document = calloc(1U, sizeof(*document));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    document->capacity = 64U;
    document->lines = calloc(document->capacity, sizeof(document->lines[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document->lines == NULL) { free(document); return UMI_STATUS_OUT_OF_MEMORY; }
    document->revision = 1U; *out_document = document; return UMI_STATUS_OK;
}
/*
 * Release or reset state held by vcs diff document so the same storage can be reused
 * safely.
 */
void umi_vcs_diff_document_destroy(UmiVcsDiffDocument *document)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (document != NULL) { free(document->lines); free(document); } }
/*
 * Release or reset state held by vcs diff document so the same storage can be reused
 * safely.
 */
void umi_vcs_diff_document_clear(UmiVcsDiffDocument *document)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) return;
    document->path[0] = '\0'; document->count = 0U; document->additions = 0U;
    document->deletions = 0U; document->hunks = 0U; document->staged = 0;
    document->binary = 0; document->revision += 1U;
}
/*
 * Read vcs diff document into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_vcs_diff_document_parse(UmiVcsDiffDocument *document, const char *path, int staged, const char *unified_diff)
{
    const char *cursor;
    size_t old_line = 0U, new_line = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || path == NULL || unified_diff == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (strlen(path) + 1U > sizeof(document->path)) return UMI_STATUS_CAPACITY_EXCEEDED;
    umi_vcs_diff_document_clear(document);
    (void)memcpy(document->path, path, strlen(path) + 1U); document->staged = staged != 0;
    cursor = unified_diff;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        size_t length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        UmiVcsDiffLine *line;
        UmiStatus capacity_status = ensure_line_capacity(document);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (capacity_status != UMI_STATUS_OK) return capacity_status;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= UMI_VCS_DIFF_TEXT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        line = &document->lines[document->count++];
        (void)memset(line, 0, sizeof(*line));
        (void)memcpy(line->text, cursor, length); line->text[length] = '\0';
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strncmp(line->text, "@@", 2U) == 0) {
            line->kind = UMI_VCS_DIFF_HUNK; parse_hunk_header(line->text, &old_line, &new_line);
            line->old_line = old_line; line->new_line = new_line; document->hunks += 1U;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strncmp(line->text, "diff ", 5U) == 0 || strncmp(line->text, "index ", 6U) == 0 ||
                   strncmp(line->text, "--- ", 4U) == 0 || strncmp(line->text, "+++ ", 4U) == 0) {
            line->kind = UMI_VCS_DIFF_HEADER;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (line->text[0] == '+' && strncmp(line->text, "+++", 3U) != 0) {
            line->kind = UMI_VCS_DIFF_ADDITION; line->new_line = new_line++; document->additions += 1U;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (line->text[0] == '-' && strncmp(line->text, "---", 3U) != 0) {
            line->kind = UMI_VCS_DIFF_DELETION; line->old_line = old_line++; document->deletions += 1U;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (line->text[0] == '\\') {
            line->kind = UMI_VCS_DIFF_NOTICE;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            line->kind = UMI_VCS_DIFF_CONTEXT; line->old_line = old_line++; line->new_line = new_line++;
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (strstr(line->text, "Binary files") != NULL) document->binary = 1;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL) break;
        cursor = end + 1;
    }
    document->revision += 1U; return UMI_STATUS_OK;
}
/*
 * Provide the vcs diff document snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_diff_document_snapshot(const UmiVcsDiffDocument *document, UmiVcsDiffDocumentSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)snprintf(out_snapshot->path, sizeof(out_snapshot->path), "%s", document->path);
    out_snapshot->staged = document->staged; out_snapshot->binary = document->binary;
    out_snapshot->lines = document->count; out_snapshot->additions = document->additions;
    out_snapshot->deletions = document->deletions; out_snapshot->hunks = document->hunks;
    out_snapshot->revision = document->revision; return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by vcs diff document line without changing
 * their state.
 */
size_t umi_vcs_diff_document_line_count(const UmiVcsDiffDocument *document)
{ return document != NULL ? document->count : 0U; }
/*
 * Find vcs diff document line while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiVcsDiffLine *umi_vcs_diff_document_line_at(const UmiVcsDiffDocument *document, size_t index)
{ return document != NULL && index < document->count ? &document->lines[index] : NULL; }
