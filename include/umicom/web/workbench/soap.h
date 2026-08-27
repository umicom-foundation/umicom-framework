/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/soap.h
 *
 * PURPOSE:
 *   Model XML/SOAP operations and build bounded SOAP envelopes and HTTP
 *   requests while leaving transport execution to the provider interface.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_SOAP_H
#define UMICOM_WEB_WORKBENCH_SOAP_H

#include "umicom/web/workbench/request.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWebWorkbenchSoapVersion {
    UMI_WEB_WORKBENCH_SOAP_11 = 0,
    UMI_WEB_WORKBENCH_SOAP_12 = 1
} UmiWebWorkbenchSoapVersion;

typedef struct UmiWebWorkbenchSoapOperation {
    char operation_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char name[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    char endpoint[UMI_WEB_WORKBENCH_URL_CAPACITY];
    char action[UMI_WEB_WORKBENCH_URL_CAPACITY];
    char namespace_uri[UMI_WEB_WORKBENCH_URL_CAPACITY];
    char body_xml[UMI_WEB_WORKBENCH_PAYLOAD_CAPACITY / 2U];
    UmiWebWorkbenchSoapVersion version;
} UmiWebWorkbenchSoapOperation;

typedef struct UmiWebWorkbenchSoapCatalogue {
    UmiWebWorkbenchSoapOperation operations[UMI_WEB_WORKBENCH_MAX_OPERATIONS];
    size_t operation_count;
    uint64_t revision;
} UmiWebWorkbenchSoapCatalogue;

void umi_web_workbench_soap_catalogue_init(
    UmiWebWorkbenchSoapCatalogue *catalogue);
UmiStatus umi_web_workbench_soap_catalogue_upsert(
    UmiWebWorkbenchSoapCatalogue *catalogue,
    const UmiWebWorkbenchSoapOperation *operation);
const UmiWebWorkbenchSoapOperation *umi_web_workbench_soap_catalogue_find(
    const UmiWebWorkbenchSoapCatalogue *catalogue,
    const char *operation_id);
UmiStatus umi_web_workbench_soap_build_envelope(
    const UmiWebWorkbenchSoapOperation *operation,
    char *out_xml,
    size_t capacity,
    size_t *out_length);
UmiStatus umi_web_workbench_soap_make_request(
    const UmiWebWorkbenchSoapOperation *operation,
    UmiWebWorkbenchRequest *out_request);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_SOAP_H */
