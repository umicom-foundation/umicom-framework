/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/workbench/sql_document.h
 *
 * PURPOSE:
 *   Maintain a revisioned SQL editor document, connection binding and selected
 *   statement range independently of any UI toolkit or SQL provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_WORKBENCH_SQL_DOCUMENT_H
#define UMICOM_DATA_WORKBENCH_SQL_DOCUMENT_H

#include "umicom/data/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data sql document data shared with callers of this public contract.
 */
typedef struct UmiDataSqlDocument {
    uint32_t struct_size;
    uint32_t api_version;
    char document_id[UMI_DATABASE_ID_CAPACITY];
    char title[UMI_DATABASE_NAME_CAPACITY];
    char connection_id[UMI_DATABASE_ID_CAPACITY];
    char dialect[UMI_DATABASE_NAME_CAPACITY];
    char text[UMI_DATABASE_SQL_CAPACITY];
    size_t text_length;
    size_t selection_start;
    size_t selection_length;
    int dirty;
    int read_only;
    uint64_t revision;
} UmiDataSqlDocument;

/**
 * Initialise data sql document from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_data_sql_document_init(
    UmiDataSqlDocument *document,
    const char *document_id,
    const char *title,
    const char *dialect);
/**
 * Provide the data sql document bind connection operation used by this module and its
 * client applications.
 */
UmiStatus umi_data_sql_document_bind_connection(
    UmiDataSqlDocument *document,
    const char *connection_id);
/**
 * Provide the data sql document set text operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_sql_document_set_text(
    UmiDataSqlDocument *document,
    const char *text,
    uint64_t expected_revision);
/**
 * Provide the data sql document set selection operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_sql_document_set_selection(
    UmiDataSqlDocument *document,
    size_t start,
    size_t length);
/**
 * Provide the data sql document statement operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_sql_document_statement(
    const UmiDataSqlDocument *document,
    char *out_statement,
    size_t capacity);
/**
 * Provide the data sql document mark saved operation used by this module and its client
 * applications.
 */
void umi_data_sql_document_mark_saved(UmiDataSqlDocument *document);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_SQL_DOCUMENT_H */
