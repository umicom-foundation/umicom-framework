/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/cvm_document_check.c
 *
 * PURPOSE:
 *   Check atomic bounded document snapshots and reject stale diagnostic projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/cvm_document_check.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

UmiStatus umi_cvm_check_document(const UmiDocumentStore *store, UmiDocumentId document,
    size_t max_source_bytes, UmiCvmDocumentCheckResult *result, size_t result_size)
{
    if (result == NULL || result_size != sizeof *result)
        return UMI_STATUS_INVALID_ARGUMENT;
    *result = (UmiCvmDocumentCheckResult){.check_status = UMI_STATUS_INVALID_ARGUMENT};
    const size_t limit = max_source_bytes < UMC_SOURCE_LIMIT ? max_source_bytes : UMC_SOURCE_LIMIT;
    char *text = NULL;
    UmiDocumentSnapshot snapshot;
    UmiStatus status = UmiDocumentStoreCopySnapshotBounded(store, document, limit, &snapshot, &text);
    if (status != UMI_STATUS_OK) {
        result->check_status = status;
        return status;
    }
    /* Copy and revision come from one store lock. The compiler receives only
     * the immutable copy, so a concurrent edit cannot change its input bytes. */
    result->owner = store;
    result->document = snapshot;
    result->captured = true;
    status = umi_cvm_check_source(text, snapshot.length, snapshot.revision,
        &result->source, sizeof result->source);
    umi_document_store_free_text(text);
    result->check_status = status;
    return status;
}

bool umi_cvm_document_check_matches(const UmiDocumentStore *store,
    const UmiCvmDocumentCheckResult *result, size_t result_size)
{
    if (store == NULL || result == NULL || result_size != sizeof *result ||
        !result->captured || result->owner != store ||
        result->source.source_revision != result->document.revision)
        return false;
    UmiDocumentSnapshot current;
    return umi_document_store_snapshot(store, result->document.document_id, &current) == UMI_STATUS_OK &&
        current.revision == result->document.revision &&
        current.length == result->document.length &&
        current.has_path == result->document.has_path &&
        strcmp(current.path, result->document.path) == 0;
}

/* Complete a temporary record before publishing to the caller. This avoids
 * partially overwritten Problems rows when a path or location cannot fit. */
static UmiStatus Project(const UmiCvmDocumentCheckResult *result, bool warning,
    size_t index, size_t line, size_t column, size_t span, const char *message,
    UmiDiagnosticSnapshot *out)
{
    if (line == 0U || column == 0U || line > UINT32_MAX ||
        column > UINT32_MAX || span > UINT32_MAX - column)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDiagnosticSnapshot row = {0};
    row.struct_size = (uint32_t)sizeof row;
    row.api_version = UMI_DIAGNOSTIC_API_VERSION;
    int n = snprintf(row.uri, sizeof row.uri, "%s", result->document.path);
    if (n < 0 || (size_t)n >= sizeof row.uri) return UMI_STATUS_CAPACITY_EXCEEDED;
    n = snprintf(row.message, sizeof row.message, "%s", message);
    if (n < 0 || (size_t)n >= sizeof row.message) return UMI_STATUS_CAPACITY_EXCEEDED;
    n = snprintf(row.id, sizeof row.id, "umicom-c:%" PRIu64 ":%" PRIu64 ":%c:%zu",
        result->document.document_id, result->document.revision, warning ? 'w' : 'e', index);
    if (n < 0 || (size_t)n >= sizeof row.id) return UMI_STATUS_CAPACITY_EXCEEDED;
    strcpy(row.source, "umicom-c");
    strcpy(row.owner, "umicom-c-source-check");
    strcpy(row.code, warning ? "UMC_LOCAL_NEVER_READ" : "UMC_SOURCE_ERROR");
    strcpy(row.tags, "source-only;byte-columns;umicom-c-subset");
    strcpy(row.detail, "Checked an in-memory Umicom C subset snapshot; no program execution or full C23 qualification.");
    row.severity = warning ? UMI_DIAGNOSTIC_WARNING : UMI_DIAGNOSTIC_ERROR;
    row.kind = UMI_DIAGNOSTIC_KIND_COMPILER;
    row.line = (uint32_t)line;
    row.column = (uint32_t)column;
    row.end_line = row.line;
    row.end_column = (uint32_t)(column + span);
    row.correlation_id = result->document.document_id;
    row.revision = result->document.revision;
    row.transient = 1;
    *out = row;
    return UMI_STATUS_OK;
}

UmiStatus umi_cvm_document_check_warning(const UmiDocumentStore *store,
    const UmiCvmDocumentCheckResult *result, size_t result_size, size_t index,
    UmiDiagnosticSnapshot *out)
{
    if (out == NULL || result == NULL || result_size != sizeof *result)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_cvm_document_check_matches(store, result, result_size))
        return UMI_STATUS_INVALID_STATE;
    if (!result->source.completed || result->check_status != UMI_STATUS_OK ||
        index >= result->source.report.warning_count)
        return UMI_STATUS_NOT_FOUND;
    if (index >= UMC_SOURCE_WARNING_LIMIT) return UMI_STATUS_INVALID_ARGUMENT;
    const UmcSourceWarning *warning = &result->source.report.warnings[index];
    if (warning->kind != UMC_WARNING_LOCAL_NEVER_READ ||
        warning->source_offset > result->document.length ||
        warning->source_length > result->document.length - warning->source_offset)
        return UMI_STATUS_INVALID_ARGUMENT;
    return Project(result, true, index, warning->line, warning->column, warning->source_length,
        "Local variable is declared but never read.", out);
}

UmiStatus umi_cvm_document_check_error(const UmiDocumentStore *store,
    const UmiCvmDocumentCheckResult *result, size_t result_size, UmiDiagnosticSnapshot *out)
{
    if (out == NULL || result == NULL || result_size != sizeof *result)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_cvm_document_check_matches(store, result, result_size))
        return UMI_STATUS_INVALID_STATE;
    if (result->check_status == UMI_STATUS_OK) return UMI_STATUS_NOT_FOUND;
    const UmcDiagnostic *error = &result->source.diagnostic;
    if (error->message == NULL) return UMI_STATUS_NOT_FOUND;
    return Project(result, false, 0U, error->line, error->column, 0U, error->message, out);
}
