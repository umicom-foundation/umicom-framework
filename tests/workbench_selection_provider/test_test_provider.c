/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection_provider/test_test_provider.c
 *
 * PURPOSE:
 *   Verify Test Platform items retain suite/framework/source/outcome metadata.
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

#include "umicom/workbench_selection_provider/test.h"

int main(void)
{
    UmiTestPlatformItemSnapshot item = {0};
    UmiWorkbenchSelection selection;
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = 2U;
    (void)strcpy(item.id, "framework.test");
    (void)strcpy(item.suite_id, "framework");
    (void)strcpy(item.name, "Framework Test");
    (void)strcpy(item.source_uri, "tests/test_framework.c");
    (void)strcpy(item.framework, "ctest");
    (void)strcpy(item.kind, "test");
    item.source_line = 20U;
    item.enabled = 1;
    item.discovered = 1;
    item.last_outcome = 1;
    item.last_duration_ms = 12.5;
    item.revision = 5U;
    assert(umi_workbench_selection_provider_from_test_item(
        &item, "studio", "studio.test-explorer",
        "workspace", 100U, &selection) == UMI_STATUS_OK);
    assert(selection.kind == UMI_WORKBENCH_SELECTION_TEST);
    assert(strcmp(selection.subject_id, "framework.test") == 0);
    assert(umi_workbench_selection_find_field(
        &selection, "framework") != NULL);

    return 0;
}
