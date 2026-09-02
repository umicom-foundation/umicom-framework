/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/language_matrix.c
 * PURPOSE: Implement the reusable polyglot project language matrix.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/language_matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiProjectLanguageMatrix {
    char project_id[128];
    char primary_language_id[UMI_PROJECT_LANGUAGE_ID_CAPACITY];
    UmiProjectLanguageEntrySnapshot entries[UMI_PROJECT_LANGUAGE_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
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
}

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiProjectLanguageMatrix *matrix,
                         const char *language_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < matrix->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(matrix->entries[index].language_id, language_id) == 0)
            return index;
    }
    return SIZE_MAX;
}

/*
 * Initialise project language entry from caller-provided values so later operations
 * receive a known state.
 */
void umi_project_language_entry_init(UmiProjectLanguageEntrySnapshot *entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL) return;
    memset(entry, 0, sizeof(*entry));
    entry->struct_size = (uint32_t)sizeof(*entry);
    entry->api_version = UMI_PROJECT_LANGUAGE_MATRIX_API_VERSION;
    entry->enabled = 1;
    entry->revision = 1U;
}

/*
 * Initialise project language matrix from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_language_matrix_create(
    const char *project_id, UmiProjectLanguageMatrix **out_matrix)
{
    UmiProjectLanguageMatrix *matrix;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (project_id == NULL || project_id[0] == '\0' || out_matrix == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_matrix = NULL;
    matrix = (UmiProjectLanguageMatrix *)calloc(1U, sizeof(*matrix));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (matrix == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    copy_text(matrix->project_id, sizeof(matrix->project_id), project_id);
    matrix->revision = 1U;
    *out_matrix = matrix;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by project language matrix so the same storage can be reused
 * safely.
 */
void umi_project_language_matrix_destroy(UmiProjectLanguageMatrix *matrix)
{
    free(matrix);
}

/*
 * Provide the project language matrix set primary operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_language_matrix_set_primary(
    UmiProjectLanguageMatrix *matrix, const char *language_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (matrix == NULL || language_id == NULL || language_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    copy_text(matrix->primary_language_id, sizeof(matrix->primary_language_id),
              language_id);
    matrix->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the project language matrix upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_project_language_matrix_upsert(
    UmiProjectLanguageMatrix *matrix,
    const UmiProjectLanguageEntrySnapshot *entry)
{
    size_t index;
    UmiProjectLanguageEntrySnapshot copy;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (matrix == NULL || entry == NULL || entry->language_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(matrix, entry->language_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (matrix->count >= UMI_PROJECT_LANGUAGE_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = matrix->count++;
    }
    copy = *entry;
    copy.struct_size = (uint32_t)sizeof(copy);
    copy.api_version = UMI_PROJECT_LANGUAGE_MATRIX_API_VERSION;
    copy.revision = matrix->entries[index].revision + 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (copy.revision == 1U) copy.revision = 1U;
    matrix->entries[index] = copy;
    matrix->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Remove project language matrix while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_language_matrix_remove(
    UmiProjectLanguageMatrix *matrix, const char *language_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (matrix == NULL || language_id == NULL || language_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(matrix, language_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < matrix->count) {
        memmove(&matrix->entries[index], &matrix->entries[index + 1U],
                (matrix->count - index - 1U) * sizeof(matrix->entries[0]));
    }
    matrix->count -= 1U;
    memset(&matrix->entries[matrix->count], 0, sizeof(matrix->entries[0]));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(matrix->primary_language_id, language_id) == 0)
        matrix->primary_language_id[0] = '\0';
    matrix->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find project language matrix while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_language_matrix_find(
    const UmiProjectLanguageMatrix *matrix, const char *language_id,
    UmiProjectLanguageEntrySnapshot *out_entry)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (matrix == NULL || language_id == NULL || out_entry == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(matrix, language_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_entry = matrix->entries[index];
    return UMI_STATUS_OK;
}

/*
 * Find project language matrix while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_language_matrix_at(
    const UmiProjectLanguageMatrix *matrix, size_t index,
    UmiProjectLanguageEntrySnapshot *out_entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (matrix == NULL || out_entry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= matrix->count) return UMI_STATUS_NOT_FOUND;
    *out_entry = matrix->entries[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the project language matrix snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_language_matrix_snapshot(
    const UmiProjectLanguageMatrix *matrix,
    UmiProjectLanguageMatrixSnapshot *out_snapshot)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (matrix == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_PROJECT_LANGUAGE_MATRIX_API_VERSION;
    copy_text(out_snapshot->project_id, sizeof(out_snapshot->project_id),
              matrix->project_id);
    copy_text(out_snapshot->primary_language_id,
              sizeof(out_snapshot->primary_language_id),
              matrix->primary_language_id);
    out_snapshot->language_count = matrix->count;
    out_snapshot->revision = matrix->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < matrix->count; ++index) {
        const UmiProjectLanguageEntrySnapshot *entry = &matrix->entries[index];
        /* Apply this operation only while the related capability or state is available. */
        if (entry->enabled != 0) out_snapshot->enabled_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->interop_policy == UMI_PROJECT_LANGUAGE_INTEROP_C_ABI)
            out_snapshot->c_abi_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->interop_policy == UMI_PROJECT_LANGUAGE_INTEROP_TRANSPILE_TO_C)
            out_snapshot->transpile_to_c_count += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Check that project language matrix satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_project_language_matrix_validate(
    const UmiProjectLanguageMatrix *matrix,
    UmiProjectLanguageValidationReport *out_report)
{
    size_t left;
    UmiProjectLanguageEntrySnapshot primary;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (matrix == NULL || out_report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_report, 0, sizeof(*out_report));
    out_report->struct_size = (uint32_t)sizeof(*out_report);
    out_report->api_version = UMI_PROJECT_LANGUAGE_MATRIX_API_VERSION;
    out_report->compile_order_is_unique = 1;
    out_report->has_primary_language = matrix->primary_language_id[0] != '\0';
    /* Apply this branch only when its contract condition is satisfied. */
    if (out_report->has_primary_language == 0) {
        out_report->error_count += 1U;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (umi_project_language_matrix_find(
                   matrix, matrix->primary_language_id, &primary) != UMI_STATUS_OK) {
        out_report->error_count += 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        out_report->primary_language_enabled = primary.enabled != 0;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (primary.enabled == 0) out_report->error_count += 1U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (left = 0U; left < matrix->count; ++left) {
        size_t right;
        /* Apply this branch only when its contract condition is satisfied. */
        if (matrix->entries[left].source_roots[0] == '\0')
            out_report->warning_count += 1U;
        /* Apply this operation only while the related capability or state is available. */
        if (matrix->entries[left].enabled == 0) continue;
        /* Visit each bounded item once so every record receives the same rule. */
        for (right = left + 1U; right < matrix->count; ++right) {
            /* Apply this operation only while the related capability or state is available. */
            if (matrix->entries[right].enabled != 0 &&
                matrix->entries[left].compile_order ==
                    matrix->entries[right].compile_order) {
                out_report->compile_order_is_unique = 0;
                out_report->error_count += 1U;
            }
        }
    }
    (void)snprintf(out_report->summary, sizeof(out_report->summary),
                   "%zu language(s), %zu error(s), %zu warning(s)",
                   matrix->count, out_report->error_count,
                   out_report->warning_count);
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by project language matrix without changing
 * their state.
 */
size_t umi_project_language_matrix_count(const UmiProjectLanguageMatrix *matrix)
{
    return matrix != NULL ? matrix->count : 0U;
}

/*
 * Provide the project language interop policy text operation used by this module and its
 * client applications.
 */
const char *umi_project_language_interop_policy_text(
    UmiProjectLanguageInteropPolicy policy)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (policy) {
    case UMI_PROJECT_LANGUAGE_INTEROP_C_ABI: return "c-abi";
    case UMI_PROJECT_LANGUAGE_INTEROP_TRANSPILE_TO_C: return "transpile-to-c";
    case UMI_PROJECT_LANGUAGE_INTEROP_FOREIGN_RUNTIME: return "foreign-runtime";
    case UMI_PROJECT_LANGUAGE_INTEROP_DATA_EXCHANGE: return "data-exchange";
    case UMI_PROJECT_LANGUAGE_INTEROP_NONE:
    default: return "none";
    }
}
