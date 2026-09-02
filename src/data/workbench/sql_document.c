/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/sql_document.c
 *
 * PURPOSE:
 *   Implement revision-safe SQL document and statement selection state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/sql_document.h"

#include <string.h>

/*
 * Initialise data sql document from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_data_sql_document_init(
    UmiDataSqlDocument *document,
    const char *document_id,
    const char *title,
    const char *dialect)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || document_id == NULL || document_id[0] == '\0' ||
        title == NULL || dialect == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(document, 0, sizeof(*document));
    document->struct_size = (uint32_t)sizeof(*document);
    document->api_version = UMI_DATA_WORKBENCH_API_VERSION;
    status = umi_data_workbench_copy_text(
        document->document_id, sizeof(document->document_id), document_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_data_workbench_copy_text(
        document->title, sizeof(document->title), title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_data_workbench_copy_text(
        document->dialect, sizeof(document->dialect), dialect);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) document->revision = 1U;
    return status;
}

/*
 * Provide the data sql document bind connection operation used by this module and its
 * client applications.
 */
UmiStatus umi_data_sql_document_bind_connection(
    UmiDataSqlDocument *document,
    const char *connection_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_data_workbench_copy_text(
        document->connection_id, sizeof(document->connection_id),
        connection_id != NULL ? connection_id : "");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) document->revision += 1U;
    return status;
}

/*
 * Provide the data sql document set text operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_sql_document_set_text(
    UmiDataSqlDocument *document,
    const char *text,
    uint64_t expected_revision)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (document->read_only) return UMI_STATUS_PERMISSION_DENIED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (expected_revision != 0U && expected_revision != document->revision) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_data_workbench_copy_text(
        document->text, sizeof(document->text), text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    document->text_length = strlen(document->text);
    document->selection_start = 0U;
    document->selection_length = 0U;
    document->dirty = 1;
    document->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the data sql document set selection operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_sql_document_set_selection(
    UmiDataSqlDocument *document,
    size_t start,
    size_t length)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || start > document->text_length ||
        length > document->text_length - start) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    document->selection_start = start;
    document->selection_length = length;
    document->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the data sql document statement operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_sql_document_statement(
    const UmiDataSqlDocument *document,
    char *out_statement,
    size_t capacity)
{
    size_t start;
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || out_statement == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    start = document->selection_length > 0U ? document->selection_start : 0U;
    length = document->selection_length > 0U
        ? document->selection_length : document->text_length;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length == 0U || length >= capacity) {
        out_statement[0] = '\0';
        return length == 0U
            ? UMI_STATUS_INVALID_STATE : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(out_statement, document->text + start, length);
    out_statement[length] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the data sql document mark saved operation used by this module and its client
 * applications.
 */
void umi_data_sql_document_mark_saved(UmiDataSqlDocument *document)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) return;
    document->dirty = 0;
    document->revision += 1U;
}
