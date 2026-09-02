/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_persistence_bridge.c
 *
 * PURPOSE:
 *   Verify the designer persistence bridge saves, loads, lists and deletes
 *   through the public layout store port.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchMemoryStore store;
    UmiWorkbenchLayoutStoreAdapter adapter;
    UmiWorkbenchDesignerPersistenceBridge bridge;
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchLayoutDocument loaded;
    UmiWorkbenchLayoutStoreQuery query;
    UmiWorkbenchLayoutStoreList list;

    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_memory_store_init(&store);
    adapter = umi_workbench_memory_store_adapter(&store);
    TEST_REQUIRE_STATUS(umi_workbench_designer_persistence_bridge_init(
        &bridge, &adapter));
    umi_workbench_designer_persistence_bridge_mark_dirty(&bridge);
    TEST_REQUIRE_STATUS(umi_workbench_designer_persistence_bridge_save(
        &bridge, &document, 0U, 1000U));
    TEST_REQUIRE(bridge.save_state == UMI_WORKBENCH_DESIGNER_SAVE_CLEAN);
    TEST_REQUIRE(umi_workbench_memory_store_layout_count(&store) == 1U);
    TEST_REQUIRE_STATUS(umi_workbench_designer_persistence_bridge_load(
        &bridge, "layout.fixture", &loaded));
    TEST_REQUIRE(strcmp(loaded.name, "Fixture Layout") == 0);
    query = umi_workbench_layout_store_query_default();
    TEST_REQUIRE_STATUS(umi_workbench_designer_persistence_bridge_list(
        &bridge, &query, &list));
    TEST_REQUIRE(list.count == 1U);
    TEST_REQUIRE_STATUS(umi_workbench_designer_persistence_bridge_delete(
        &bridge, "layout.fixture", bridge.persisted_revision));
    TEST_REQUIRE(umi_workbench_memory_store_layout_count(&store) == 0U);
    umi_workbench_memory_store_clear(&store);
    return 0;
}
