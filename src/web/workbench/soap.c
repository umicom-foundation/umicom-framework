/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/soap.c
 *
 * PURPOSE:
 *   Implement SOAP operation catalogues, envelopes and executable request
 *   models over the existing provider-neutral transport seam.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/soap.h"

#include <stdio.h>
#include <string.h>

/* Provide the operation index operation used by this module and its client applications. */
static size_t operation_index(
    const UmiWebWorkbenchSoapCatalogue *catalogue,
    const char *operation_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->operation_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->operations[index].operation_id, operation_id) == 0) {
            return index;
        }
    }
    return catalogue->operation_count;
}

/*
 * Initialise web workbench soap catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_web_workbench_soap_catalogue_init(
    UmiWebWorkbenchSoapCatalogue *catalogue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return;
    memset(catalogue, 0, sizeof(*catalogue));
    catalogue->revision = 1U;
}

/*
 * Provide the web workbench soap catalogue upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_soap_catalogue_upsert(
    UmiWebWorkbenchSoapCatalogue *catalogue,
    const UmiWebWorkbenchSoapOperation *operation)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || operation == NULL || operation->operation_id[0] == '\0' ||
        operation->name[0] == '\0' || operation->endpoint[0] == '\0' ||
        operation->namespace_uri[0] == '\0' ||
        operation->version > UMI_WEB_WORKBENCH_SOAP_12) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = operation_index(catalogue, operation->operation_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == catalogue->operation_count) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index >= UMI_WEB_WORKBENCH_MAX_OPERATIONS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        catalogue->operation_count++;
    }
    catalogue->operations[index] = *operation;
    catalogue->revision++;
    return UMI_STATUS_OK;
}

/*
 * Find web workbench soap catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiWebWorkbenchSoapOperation *umi_web_workbench_soap_catalogue_find(
    const UmiWebWorkbenchSoapCatalogue *catalogue,
    const char *operation_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || operation_id == NULL) return NULL;
    index = operation_index(catalogue, operation_id);
    return index < catalogue->operation_count ? &catalogue->operations[index] : NULL;
}

/*
 * Provide the web workbench soap build envelope operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_soap_build_envelope(
    const UmiWebWorkbenchSoapOperation *operation,
    char *out_xml,
    size_t capacity,
    size_t *out_length)
{
    const char *envelope_namespace;
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation == NULL || out_xml == NULL || capacity == 0U ||
        operation->namespace_uri[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    envelope_namespace = operation->version == UMI_WEB_WORKBENCH_SOAP_12
        ? "http://www.w3.org/2003/05/soap-envelope"
        : "http://schemas.xmlsoap.org/soap/envelope/";
    written = snprintf(out_xml, capacity,
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
        "<soap:Envelope xmlns:soap=\"%s\" xmlns:umi=\"%s\">"
        "<soap:Header/>"
        "<soap:Body><umi:%s>%s</umi:%s></soap:Body>"
        "</soap:Envelope>",
        envelope_namespace, operation->namespace_uri, operation->name,
        operation->body_xml, operation->name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_length != NULL) *out_length = (size_t)written;
    return UMI_STATUS_OK;
}

/*
 * Provide the web workbench soap make request operation used by this module and its client
 * applications.
 */
UmiStatus umi_web_workbench_soap_make_request(
    const UmiWebWorkbenchSoapOperation *operation,
    UmiWebWorkbenchRequest *out_request)
{
    char envelope[UMI_WEB_WORKBENCH_PAYLOAD_CAPACITY];
    char content_type[UMI_WEB_HEADER_VALUE_CAPACITY];
    UmiStatus status;
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation == NULL || out_request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_web_workbench_soap_build_envelope(operation, envelope,
        sizeof(envelope), NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    umi_web_workbench_request_init(out_request, operation->operation_id,
        operation->name, UMI_HTTP_METHOD_POST, operation->endpoint);
    status = umi_web_workbench_request_set_body(out_request,
        UMI_WEB_WORKBENCH_BODY_XML, envelope);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (operation->version == UMI_WEB_WORKBENCH_SOAP_12) {
        written = snprintf(content_type, sizeof(content_type),
            "application/soap+xml; charset=utf-8; action=\"%s\"", operation->action);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(content_type)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        return umi_web_workbench_request_set_header(out_request,
            "Content-Type", content_type);
    }
    status = umi_web_workbench_request_set_header(out_request,
        "Content-Type", "text/xml; charset=utf-8");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_web_workbench_request_set_header(out_request,
        "SOAPAction", operation->action);
}
