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

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
}

static size_t find_index(const UmiProjectLanguageMatrix *matrix,
                         const char *language_id)
{
    size_t index;
    for (index = 0U; index < matrix->count; ++index) {
        if (strcmp(matrix->entries[index].language_id, language_id) == 0)
            return index;
    }
    return SIZE_MAX;
}

void umi_project_language_entry_init(UmiProjectLanguageEntrySnapshot *entry)
{
    if (entry == NULL) return;
    memset(entry, 0, sizeof(*entry));
    entry->struct_size = (uint32_t)sizeof(*entry);
    entry->api_version = UMI_PROJECT_LANGUAGE_MATRIX_API_VERSION;
    entry->enabled = 1;
    entry->revision = 1U;
}

UmiStatus umi_project_language_matrix_create(
    const char *project_id, UmiProjectLanguageMatrix **out_matrix)
{
    UmiProjectLanguageMatrix *matrix;
    if (project_id == NULL || project_id[0] == '\0' || out_matrix == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_matrix = NULL;
    matrix = (UmiProjectLanguageMatrix *)calloc(1U, sizeof(*matrix));
    if (matrix == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    copy_text(matrix->project_id, sizeof(matrix->project_id), project_id);
    matrix->revision = 1U;
    *out_matrix = matrix;
    return UMI_STATUS_OK;
}

void umi_project_language_matrix_destroy(UmiProjectLanguageMatrix *matrix)
{
    free(matrix);
}

UmiStatus umi_project_language_matrix_set_primary(
    UmiProjectLanguageMatrix *matrix, const char *language_id)
{
    if (matrix == NULL || language_id == NULL || language_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    copy_text(matrix->primary_language_id, sizeof(matrix->primary_language_id),
              language_id);
    matrix->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_project_language_matrix_upsert(
    UmiProjectLanguageMatrix *matrix,
    const UmiProjectLanguageEntrySnapshot *entry)
{
    size_t index;
    UmiProjectLanguageEntrySnapshot copy;
    if (matrix == NULL || entry == NULL || entry->language_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(matrix, entry->language_id);
    if (index == SIZE_MAX) {
        if (matrix->count >= UMI_PROJECT_LANGUAGE_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = matrix->count++;
    }
    copy = *entry;
    copy.struct_size = (uint32_t)sizeof(copy);
    copy.api_version = UMI_PROJECT_LANGUAGE_MATRIX_API_VERSION;
    copy.revision = matrix->entries[index].revision + 1U;
    if (copy.revision == 1U) copy.revision = 1U;
    matrix->entries[index] = copy;
    matrix->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_project_language_matrix_remove(
    UmiProjectLanguageMatrix *matrix, const char *language_id)
{
    size_t index;
    if (matrix == NULL || language_id == NULL || language_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(matrix, language_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < matrix->count) {
        memmove(&matrix->entries[index], &matrix->entries[index + 1U],
                (matrix->count - index - 1U) * sizeof(matrix->entries[0]));
    }
    matrix->count -= 1U;
    memset(&matrix->entries[matrix->count], 0, sizeof(matrix->entries[0]));
    if (strcmp(matrix->primary_language_id, language_id) == 0)
        matrix->primary_language_id[0] = '\0';
    matrix->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_project_language_matrix_find(
    const UmiProjectLanguageMatrix *matrix, const char *language_id,
    UmiProjectLanguageEntrySnapshot *out_entry)
{
    size_t index;
    if (matrix == NULL || language_id == NULL || out_entry == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(matrix, language_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_entry = matrix->entries[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_project_language_matrix_at(
    const UmiProjectLanguageMatrix *matrix, size_t index,
    UmiProjectLanguageEntrySnapshot *out_entry)
{
    if (matrix == NULL || out_entry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= matrix->count) return UMI_STATUS_NOT_FOUND;
    *out_entry = matrix->entries[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_project_language_matrix_snapshot(
    const UmiProjectLanguageMatrix *matrix,
    UmiProjectLanguageMatrixSnapshot *out_snapshot)
{
    size_t index;
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
    for (index = 0U; index < matrix->count; ++index) {
        const UmiProjectLanguageEntrySnapshot *entry = &matrix->entries[index];
        if (entry->enabled != 0) out_snapshot->enabled_count += 1U;
        if (entry->interop_policy == UMI_PROJECT_LANGUAGE_INTEROP_C_ABI)
            out_snapshot->c_abi_count += 1U;
        if (entry->interop_policy == UMI_PROJECT_LANGUAGE_INTEROP_TRANSPILE_TO_C)
            out_snapshot->transpile_to_c_count += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_project_language_matrix_validate(
    const UmiProjectLanguageMatrix *matrix,
    UmiProjectLanguageValidationReport *out_report)
{
    size_t left;
    UmiProjectLanguageEntrySnapshot primary;
    if (matrix == NULL || out_report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_report, 0, sizeof(*out_report));
    out_report->struct_size = (uint32_t)sizeof(*out_report);
    out_report->api_version = UMI_PROJECT_LANGUAGE_MATRIX_API_VERSION;
    out_report->compile_order_is_unique = 1;
    out_report->has_primary_language = matrix->primary_language_id[0] != '\0';
    if (out_report->has_primary_language == 0) {
        out_report->error_count += 1U;
    } else if (umi_project_language_matrix_find(
                   matrix, matrix->primary_language_id, &primary) != UMI_STATUS_OK) {
        out_report->error_count += 1U;
    } else {
        out_report->primary_language_enabled = primary.enabled != 0;
        if (primary.enabled == 0) out_report->error_count += 1U;
    }
    for (left = 0U; left < matrix->count; ++left) {
        size_t right;
        if (matrix->entries[left].source_roots[0] == '\0')
            out_report->warning_count += 1U;
        if (matrix->entries[left].enabled == 0) continue;
        for (right = left + 1U; right < matrix->count; ++right) {
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

size_t umi_project_language_matrix_count(const UmiProjectLanguageMatrix *matrix)
{
    return matrix != NULL ? matrix->count : 0U;
}

const char *umi_project_language_interop_policy_text(
    UmiProjectLanguageInteropPolicy policy)
{
    switch (policy) {
    case UMI_PROJECT_LANGUAGE_INTEROP_C_ABI: return "c-abi";
    case UMI_PROJECT_LANGUAGE_INTEROP_TRANSPILE_TO_C: return "transpile-to-c";
    case UMI_PROJECT_LANGUAGE_INTEROP_FOREIGN_RUNTIME: return "foreign-runtime";
    case UMI_PROJECT_LANGUAGE_INTEROP_DATA_EXCHANGE: return "data-exchange";
    case UMI_PROJECT_LANGUAGE_INTEROP_NONE:
    default: return "none";
    }
}
