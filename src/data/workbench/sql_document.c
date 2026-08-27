/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/sql_document.c
 *
 * PURPOSE:
 *   Implement revision-safe SQL document and statement selection state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/sql_document.h"

#include <string.h>

UmiStatus umi_data_sql_document_init(
    UmiDataSqlDocument *document,
    const char *document_id,
    const char *title,
    const char *dialect)
{
    UmiStatus status;
    if (document == NULL || document_id == NULL || document_id[0] == '\0' ||
        title == NULL || dialect == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(document, 0, sizeof(*document));
    document->struct_size = (uint32_t)sizeof(*document);
    document->api_version = UMI_DATA_WORKBENCH_API_VERSION;
    status = umi_data_workbench_copy_text(
        document->document_id, sizeof(document->document_id), document_id);
    if (status == UMI_STATUS_OK) status = umi_data_workbench_copy_text(
        document->title, sizeof(document->title), title);
    if (status == UMI_STATUS_OK) status = umi_data_workbench_copy_text(
        document->dialect, sizeof(document->dialect), dialect);
    if (status == UMI_STATUS_OK) document->revision = 1U;
    return status;
}

UmiStatus umi_data_sql_document_bind_connection(
    UmiDataSqlDocument *document,
    const char *connection_id)
{
    UmiStatus status;
    if (document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_data_workbench_copy_text(
        document->connection_id, sizeof(document->connection_id),
        connection_id != NULL ? connection_id : "");
    if (status == UMI_STATUS_OK) document->revision += 1U;
    return status;
}

UmiStatus umi_data_sql_document_set_text(
    UmiDataSqlDocument *document,
    const char *text,
    uint64_t expected_revision)
{
    UmiStatus status;
    if (document == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (document->read_only) return UMI_STATUS_PERMISSION_DENIED;
    if (expected_revision != 0U && expected_revision != document->revision) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_data_workbench_copy_text(
        document->text, sizeof(document->text), text);
    if (status != UMI_STATUS_OK) return status;
    document->text_length = strlen(document->text);
    document->selection_start = 0U;
    document->selection_length = 0U;
    document->dirty = 1;
    document->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_data_sql_document_set_selection(
    UmiDataSqlDocument *document,
    size_t start,
    size_t length)
{
    if (document == NULL || start > document->text_length ||
        length > document->text_length - start) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    document->selection_start = start;
    document->selection_length = length;
    document->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_data_sql_document_statement(
    const UmiDataSqlDocument *document,
    char *out_statement,
    size_t capacity)
{
    size_t start;
    size_t length;
    if (document == NULL || out_statement == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    start = document->selection_length > 0U ? document->selection_start : 0U;
    length = document->selection_length > 0U
        ? document->selection_length : document->text_length;
    if (length == 0U || length >= capacity) {
        out_statement[0] = '\0';
        return length == 0U
            ? UMI_STATUS_INVALID_STATE : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(out_statement, document->text + start, length);
    out_statement[length] = '\0';
    return UMI_STATUS_OK;
}

void umi_data_sql_document_mark_saved(UmiDataSqlDocument *document)
{
    if (document == NULL) return;
    document->dirty = 0;
    document->revision += 1U;
}
