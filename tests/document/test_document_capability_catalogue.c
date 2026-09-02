/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_document_capability_catalogue.c
 * PURPOSE: Verify the complete 96-capability document inventory and queries.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/document.h"

#include <assert.h>
#include <stdio.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDocumentCapabilityCatalog *catalog = NULL;
    UmiDocumentCapabilityCatalogSnapshot snapshot;
    UmiDocumentCapabilityQueryResult result;
    size_t index;
    assert(umi_document_capability_catalog_create(&catalog) == UMI_STATUS_OK);
    assert(umi_document_capability_catalog_register_builtins(catalog) == UMI_STATUS_OK);
    assert(umi_document_capability_catalog_count(catalog) == 96U);
    assert(umi_document_capability_catalog_snapshot(catalog, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.total_count == 96U);
    assert(snapshot.implemented_count > 40U);
    assert(snapshot.headless_count == 96U);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_DOCUMENT_CAPABILITY_CATEGORY_COUNT; ++index) {
        assert(snapshot.category_counts[index] == 8U);
    }
    assert(umi_document_capability_catalog_query_category(
        catalog, UMI_DOCUMENT_CAPABILITY_CATEGORY_ENCODING, &result) == UMI_STATUS_OK);
    assert(result.count == 8U);
    umi_document_capability_catalog_destroy(catalog);
    (void)printf("document capability catalogue passed\n");
    return 0;
}
