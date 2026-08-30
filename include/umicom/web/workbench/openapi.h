/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/openapi.h
 *
 * PURPOSE:
 *   Import and expose a bounded OpenAPI JSON operation catalogue without
 *   replacing the Integration Fabric API-contract authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_OPENAPI_H
#define UMICOM_WEB_WORKBENCH_OPENAPI_H

#include "umicom/web/workbench/request.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWebWorkbenchOpenApiOperation {
    char operation_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char summary[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    char tag[UMI_WEB_WORKBENCH_ID_CAPACITY];
    UmiHttpMethod method;
    char path[UMI_WEB_PATH_CAPACITY];
    bool deprecated;
} UmiWebWorkbenchOpenApiOperation;

typedef struct UmiWebWorkbenchOpenApiDocument {
    char document_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char specification[32U];
    char title[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    char version[64U];
    char server_url[UMI_WEB_WORKBENCH_URL_CAPACITY];
    UmiWebWorkbenchOpenApiOperation operations[
        UMI_WEB_WORKBENCH_MAX_OPERATIONS];
    size_t operation_count;
    size_t skipped_operation_count;
    uint64_t revision;
} UmiWebWorkbenchOpenApiDocument;

void umi_web_workbench_openapi_init(
    UmiWebWorkbenchOpenApiDocument *document,
    const char *document_id);
UmiStatus umi_web_workbench_openapi_add_operation(
    UmiWebWorkbenchOpenApiDocument *document,
    const UmiWebWorkbenchOpenApiOperation *operation);
UmiStatus umi_web_workbench_openapi_import_json(
    UmiWebWorkbenchOpenApiDocument *document,
    const char *json_text,
    char *out_message,
    size_t message_capacity);
const UmiWebWorkbenchOpenApiOperation *umi_web_workbench_openapi_find(
    const UmiWebWorkbenchOpenApiDocument *document,
    const char *operation_id);
size_t umi_web_workbench_openapi_query(
    const UmiWebWorkbenchOpenApiDocument *document,
    const char *text,
    const char *tag,
    const UmiWebWorkbenchOpenApiOperation **out_operations,
    size_t capacity);
UmiStatus umi_web_workbench_openapi_make_request(
    const UmiWebWorkbenchOpenApiDocument *document,
    const char *operation_id,
    UmiWebWorkbenchRequest *out_request);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_OPENAPI_H */
