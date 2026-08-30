/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_openapi.c
 * PURPOSE: Verify bounded OpenAPI JSON import, search and request generation.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/web/workbench/openapi.h"

int main(void)
{
    static const char json[] =
        "{\"openapi\":\"3.1.0\",\"info\":{\"title\":\"Trading API\",\"version\":\"1\"},"
        "\"servers\":[{\"url\":\"https://api.example.test\"}],\"paths\":{"
        "\"/orders\":{\"get\":{\"operationId\":\"listOrders\",\"summary\":\"List orders\","
        "\"tags\":[\"Trading\"]},\"post\":{\"operationId\":\"placeOrder\",\"deprecated\":false}}}}";
    UmiWebWorkbenchOpenApiDocument *document = calloc(1U, sizeof(*document));
    UmiWebWorkbenchRequest request;
    const UmiWebWorkbenchOpenApiOperation *matches[4U];
    char message[128U];
    assert(document != NULL);
    umi_web_workbench_openapi_init(document, "trading");
    assert(umi_web_workbench_openapi_import_json(document, json, message,
        sizeof(message)) == UMI_STATUS_OK);
    assert(document->operation_count == 2U);
    assert(strcmp(document->title, "Trading API") == 0);
    assert(umi_web_workbench_openapi_query(document, "orders", "Trading",
        matches, 4U) == 1U);
    assert(umi_web_workbench_openapi_make_request(document, "placeOrder",
        &request) == UMI_STATUS_OK);
    assert(strcmp(request.url, "https://api.example.test/orders") == 0);
    free(document);
    return 0;
}
