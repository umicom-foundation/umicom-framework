/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/workbench/sql_document.h
 *
 * PURPOSE:
 *   Maintain a revisioned SQL editor document, connection binding and selected
 *   statement range independently of any UI toolkit or SQL provider.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_WORKBENCH_SQL_DOCUMENT_H
#define UMICOM_DATA_WORKBENCH_SQL_DOCUMENT_H

#include "umicom/data/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

UmiStatus umi_data_sql_document_init(
    UmiDataSqlDocument *document,
    const char *document_id,
    const char *title,
    const char *dialect);
UmiStatus umi_data_sql_document_bind_connection(
    UmiDataSqlDocument *document,
    const char *connection_id);
UmiStatus umi_data_sql_document_set_text(
    UmiDataSqlDocument *document,
    const char *text,
    uint64_t expected_revision);
UmiStatus umi_data_sql_document_set_selection(
    UmiDataSqlDocument *document,
    size_t start,
    size_t length);
UmiStatus umi_data_sql_document_statement(
    const UmiDataSqlDocument *document,
    char *out_statement,
    size_t capacity);
void umi_data_sql_document_mark_saved(UmiDataSqlDocument *document);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_SQL_DOCUMENT_H */
