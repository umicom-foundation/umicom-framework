/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/soap.h
 *
 * PURPOSE:
 *   Model XML/SOAP operations and build bounded SOAP envelopes and HTTP
 *   requests while leaving transport execution to the provider interface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_SOAP_H
#define UMICOM_WEB_WORKBENCH_SOAP_H

#include "umicom/web/workbench/request.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named web workbench soap version values accepted by this public contract.
 */
typedef enum UmiWebWorkbenchSoapVersion {
    UMI_WEB_WORKBENCH_SOAP_11 = 0,
    UMI_WEB_WORKBENCH_SOAP_12 = 1
} UmiWebWorkbenchSoapVersion;

/**
 * Represent the web workbench soap operation data shared with callers of this public
 * contract.
 */
typedef struct UmiWebWorkbenchSoapOperation {
    char operation_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char name[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    char endpoint[UMI_WEB_WORKBENCH_URL_CAPACITY];
    char action[UMI_WEB_WORKBENCH_URL_CAPACITY];
    char namespace_uri[UMI_WEB_WORKBENCH_URL_CAPACITY];
    char body_xml[UMI_WEB_WORKBENCH_PAYLOAD_CAPACITY / 2U];
    UmiWebWorkbenchSoapVersion version;
} UmiWebWorkbenchSoapOperation;

/**
 * Represent the web workbench soap catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiWebWorkbenchSoapCatalogue {
    UmiWebWorkbenchSoapOperation operations[UMI_WEB_WORKBENCH_MAX_OPERATIONS];
    size_t operation_count;
    uint64_t revision;
} UmiWebWorkbenchSoapCatalogue;

/**
 * Initialise web workbench soap catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_web_workbench_soap_catalogue_init(
    UmiWebWorkbenchSoapCatalogue *catalogue);
/**
 * Provide the web workbench soap catalogue upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_soap_catalogue_upsert(
    UmiWebWorkbenchSoapCatalogue *catalogue,
    const UmiWebWorkbenchSoapOperation *operation);
/**
 * Find web workbench soap catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiWebWorkbenchSoapOperation *umi_web_workbench_soap_catalogue_find(
    const UmiWebWorkbenchSoapCatalogue *catalogue,
    const char *operation_id);
/**
 * Provide the web workbench soap build envelope operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_soap_build_envelope(
    const UmiWebWorkbenchSoapOperation *operation,
    char *out_xml,
    size_t capacity,
    size_t *out_length);
/**
 * Provide the web workbench soap make request operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_soap_make_request(
    const UmiWebWorkbenchSoapOperation *operation,
    UmiWebWorkbenchRequest *out_request);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_SOAP_H */
