/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_soap.c
 * PURPOSE: Verify SOAP catalogue, envelope and HTTP request composition.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/web/workbench/soap.h"

int main(void)
{
    UmiWebWorkbenchSoapCatalogue catalogue;
    UmiWebWorkbenchSoapOperation operation;
    UmiWebWorkbenchRequest request;
    memset(&operation, 0, sizeof(operation));
    assert(umi_web_workbench_copy_text(operation.operation_id,
        sizeof(operation.operation_id), "getPrice") == UMI_STATUS_OK);
    assert(umi_web_workbench_copy_text(operation.name,
        sizeof(operation.name), "GetPrice") == UMI_STATUS_OK);
    assert(umi_web_workbench_copy_text(operation.endpoint,
        sizeof(operation.endpoint), "https://soap.example.test/pricing") == UMI_STATUS_OK);
    assert(umi_web_workbench_copy_text(operation.action,
        sizeof(operation.action), "urn:GetPrice") == UMI_STATUS_OK);
    assert(umi_web_workbench_copy_text(operation.namespace_uri,
        sizeof(operation.namespace_uri), "urn:pricing") == UMI_STATUS_OK);
    assert(umi_web_workbench_copy_text(operation.body_xml,
        sizeof(operation.body_xml), "<umi:Symbol>EURUSD</umi:Symbol>") == UMI_STATUS_OK);
    operation.version = UMI_WEB_WORKBENCH_SOAP_11;
    umi_web_workbench_soap_catalogue_init(&catalogue);
    assert(umi_web_workbench_soap_catalogue_upsert(&catalogue, &operation) == UMI_STATUS_OK);
    assert(umi_web_workbench_soap_make_request(&operation, &request) == UMI_STATUS_OK);
    assert(strstr(request.body, "soap:Envelope") != NULL);
    assert(strcmp(umi_web_workbench_request_header(&request, "SOAPAction"),
        "urn:GetPrice") == 0);
    return 0;
}
