/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_test_bridge.c
 *
 * PURPOSE:
 *   Integration regression coverage for test bridge.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ide_integration/test_bridge.h"
int main(void)
{
    UmiTestPlatformService *tests = NULL;
    UmiTestPlatformItemSnapshot item = {0};
    UmiIdeNavigationTarget target;
    assert(umi_test_platform_service_create(&tests) == UMI_STATUS_OK);
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_TEST_PLATFORM_ITEM_API_VERSION;
    (void)strcpy(item.id, "test.a");
    (void)strcpy(item.name, "test a");
    (void)strcpy(item.source_uri, "file:///tmp/test_a.c");
    item.source_line = 12U;
    assert(umi_test_platform_item_registry_upsert(
        umi_test_platform_service_item(tests), &item) == UMI_STATUS_OK);
    assert(umi_ide_test_item_target(tests, "test.a", &target) == UMI_STATUS_OK);
    assert(target.location.line == 12U);
    umi_test_platform_service_destroy(tests);
    return 0;
}

