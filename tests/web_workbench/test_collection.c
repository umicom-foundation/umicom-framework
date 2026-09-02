/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_collection.c
 * PURPOSE: Verify revision-safe API collection updates and folder queries.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include "umicom/web/workbench/collection.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWebWorkbenchCollection *collection = calloc(1U, sizeof(*collection));
    UmiWebWorkbenchRequest request;
    const UmiWebWorkbenchRequest *matches[4U];
    uint64_t revision;
    assert(collection != NULL);
    umi_web_workbench_collection_init(collection, "trading", "Trading APIs");
    umi_web_workbench_request_init(&request, "orders", "Orders", UMI_HTTP_METHOD_GET,
        "https://api.example.test/orders");
    assert(umi_web_workbench_copy_text(request.folder, sizeof(request.folder), "Trading") == UMI_STATUS_OK);
    revision = collection->revision;
    assert(umi_web_workbench_collection_upsert(collection, &request, revision) == UMI_STATUS_OK);
    assert(umi_web_workbench_collection_upsert(collection, &request, revision) == UMI_STATUS_BUSY);
    assert(umi_web_workbench_collection_query(collection, "order", "Trading",
        matches, 4U) == 1U);
    revision = collection->revision;
    assert(umi_web_workbench_collection_remove(collection, "orders", revision) == UMI_STATUS_OK);
    free(collection);
    return 0;
}
