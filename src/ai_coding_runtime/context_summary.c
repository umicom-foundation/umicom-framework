/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/context_summary.c
 *
 * PURPOSE:
 *   Format repository context provenance without source content.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/context_summary.h"
#include "umicom/base/snapshot_validation.h"
#include "umicom/base/utf8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Provide the ai coding context summary operation used by this module and its client
 * applications.
 */
/* Bound summary metadata and stage formatted text before replacing the caller buffer; retain the metadata-only format and its prior implementation. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_context_summary(
    const UmiAiCodingMaterializedContext *context,
    char *out_text,
    size_t capacity)
{
    size_t index;
    size_t used = 0U;
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(
        out_text,
        capacity,
        "Context files: %zu, skipped: %zu, estimated tokens: %u%s\n",
        context->file_count,
        context->skipped_count,
        context->estimated_tokens,
        context->truncated ? ", truncated" : "");

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    used = (size_t)written;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < context->file_count; ++index) {
        const UmiAiCodingMaterializedFile *file = &context->files[index];

        written = snprintf(
            out_text + used,
            capacity - used,
            "%s%s [%s] %zu bytes hash=%016llx\n",
            file->active ? "* " : "  ",
            file->path,
            file->language_id,
            file->length,
            (unsigned long long)file->hash);

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= capacity - used) {
            out_text[capacity - 1U] = '\0';
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        used += (size_t)written;
    }

    return UMI_STATUS_OK;
}

#endif
static UmiStatus build_context_summary(
    const UmiAiCodingMaterializedContext *context,
    char *out_text,
    size_t capacity)
{
    size_t index;
    size_t used = 0U;
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(
        out_text,
        capacity,
        "Context files: %zu, skipped: %zu, estimated tokens: %u%s\n",
        context->file_count,
        context->skipped_count,
        context->estimated_tokens,
        context->truncated ? ", truncated" : "");

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    used = (size_t)written;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < context->file_count; ++index) {
        const UmiAiCodingMaterializedFile *file = &context->files[index];

        written = snprintf(
            out_text + used,
            capacity - used,
            "%s%s [%s] %zu bytes hash=%016llx\n",
            file->active ? "* " : "  ",
            file->path,
            file->language_id,
            file->length,
            (unsigned long long)file->hash);

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= capacity - used) {
            out_text[capacity - 1U] = '\0';
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        used += (size_t)written;
    }

    return UMI_STATUS_OK;
}


/* A summary is a metadata projection, so it must not require the source bytes
 * or claim to verify the displayed hash. Bound only the fields it consumes. */
static UmiStatus summary_metadata_validate(const UmiAiCodingMaterializedContext *context)
{
    static const UmiSnapshotTextField fields[] = {
        {"path", offsetof(UmiAiCodingMaterializedFile, path), sizeof(((UmiAiCodingMaterializedFile *)0)->path), 1},
        {"language_id", offsetof(UmiAiCodingMaterializedFile, language_id), sizeof(((UmiAiCodingMaterializedFile *)0)->language_id), 0}
    };
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (context->file_count > UMI_AI_CODING_RUNTIME_CONTEXT_CAPACITY)
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < context->file_count; ++i) {
        const UmiAiCodingMaterializedFile *file = &context->files[i];
        if (UmiSnapshotValidateTextFields(file, sizeof *file, fields,
                sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
            !umi_ai_coding_path_is_safe_relative(file->path))
            return UMI_STATUS_INVALID_STATE;
        /* Each record occupies one readable line; labels must not introduce
         * extra lines or terminal control bytes into that representation. */
        const char *labels[] = {file->path, file->language_id};
        for (size_t label = 0U; label < sizeof labels / sizeof labels[0]; ++label)
            for (const unsigned char *p = (const unsigned char *)labels[label]; *p != 0U; ++p)
                if (*p < 0x20U || *p == 0x7fU) return UMI_STATUS_INVALID_STATE;
        if (!UmiUtf8Validate((const unsigned char *)file->path, strlen(file->path), NULL) ||
            !UmiUtf8Validate((const unsigned char *)file->language_id, strlen(file->language_id), NULL))
            return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_context_summary(
    const UmiAiCodingMaterializedContext *context,
    char *out_text,
    size_t capacity)
{
    UmiStatus status;
    char *candidate;
    size_t length;
    /* Three decimal digits per byte safely bound any size_t decimal field.
     * The remaining allowance covers labels, hash digits and punctuation.
     * Counts are admitted before this bounded allocation is calculated. */
    const size_t number_space = 3U * sizeof(size_t);
    size_t candidate_capacity;
    if (out_text == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = summary_metadata_validate(context);
    if (status != UMI_STATUS_OK) return status;
    candidate_capacity = 128U + 2U * number_space +
        context->file_count * (sizeof context->files[0].path +
            sizeof context->files[0].language_id + number_space + 64U);
    candidate = malloc(candidate_capacity);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = build_context_summary(context, candidate, candidate_capacity);
    if (status == UMI_STATUS_OK) {
        length = strlen(candidate);
        if (length >= capacity) status = UMI_STATUS_CAPACITY_EXCEEDED;
        else memmove(out_text, candidate, length + 1U);
    }
    free(candidate);
    return status;
}
