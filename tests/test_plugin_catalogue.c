/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_plugin_catalogue.c
 *
 * PURPOSE:
 *   Verify extension catalogue filtering, compatibility and update detection.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "plugin_v2_fixture.h"

int main(void)
{
    UmiPluginCatalogue *catalogue = NULL;
    UmiPluginCatalogueEntry entry;
    UmiPluginCatalogueEntry results[4];
    UmiPluginCatalogueQuery query;
    (void)memset(&entry, 0, sizeof(entry));
    (void)strcpy(entry.plugin_id, "org.umicom.c-tools");
    (void)strcpy(entry.display_name, "Umicom C Tools");
    (void)strcpy(entry.publisher, "Umicom Foundation");
    (void)strcpy(entry.description, "C23 language and build tools");
    entry.version = plugin_test_version(2U, 1U, 0U);
    entry.required_abi = UMICOM_FRAMEWORK_ABI_VERSION;
    entry.verified = 1;
    assert(umi_plugin_catalogue_create(&catalogue) == UMI_STATUS_OK);
    assert(umi_plugin_catalogue_add(catalogue, &entry) == UMI_STATUS_OK);
    (void)memset(&query, 0, sizeof(query));
    query.text = "c23";
    query.framework_abi = UMICOM_FRAMEWORK_ABI_VERSION;
    query.verified_only = 1;
    assert(umi_plugin_catalogue_query(catalogue, &query, results, 4U) == 1U);
    assert(umi_plugin_catalogue_update_available(&entry,
        plugin_test_version(2U, 0U, 9U)) == 1);
    umi_plugin_catalogue_destroy(catalogue);
    return 0;
}
