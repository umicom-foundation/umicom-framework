/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_explorer.c
 *
 * PURPOSE:
 *   Exercise one Batch 23 unified-workbench contract through its public C API.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * These tests are intentionally small. They document expected behaviour without starting a graphical application.
 */

#include "umicom/umicom.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiExplorerModel *model = NULL;
    UmiUiExplorerNode node = {0};
    UmiUiExplorerSearchResults results;
    UmiUiExplorerNode found;

    assert(umi_ui_explorer_model_create(&model) == UMI_STATUS_OK);

    (void)snprintf(node.node_id, sizeof(node.node_id), "%s", "file.workbench");
    (void)snprintf(node.name, sizeof(node.name), "%s", "workbench.c");
    (void)snprintf(node.path, sizeof(node.path), "%s",
                   "applications/studio/src/app/workbench.c");
    node.kind = UMI_UI_EXPLORER_FILE;

    assert(umi_ui_explorer_model_upsert(model, &node) == UMI_STATUS_OK);
    assert(umi_ui_explorer_model_select(model, node.node_id) == UMI_STATUS_OK);
    assert(umi_ui_explorer_search(model, "workb", &results) == UMI_STATUS_OK);
    assert(results.count == 1U);
    assert(umi_ui_explorer_model_find(model, node.node_id, &found) == UMI_STATUS_OK);
    assert(found.selected == 1);

    umi_ui_explorer_model_destroy(model);

    /* File Index rows can be presented with folder context without reading the
     * filesystem or changing the indexed path. */
    {
        UmiFileIndexEntry entry = {0};
        UmiUiExplorerFilePresentation presentation;

        (void)snprintf(entry.name, sizeof(entry.name), "%s", "main.c");
        (void)snprintf(
            entry.relative_path,
            sizeof(entry.relative_path),
            "%s",
            "src/app/main.c");
        assert(UmiUiExplorerDescribeFileIndexEntry(
            &entry, &presentation) == UMI_STATUS_OK);
        assert(strcmp(presentation.name, "main.c") == 0);
        assert(strcmp(presentation.parentPath, "src/app") == 0);
        assert(strcmp(presentation.topLevel, "src") == 0);
        assert(presentation.depth == 2U);
        assert(presentation.workspaceRoot == 0);

        (void)snprintf(
            entry.relative_path,
            sizeof(entry.relative_path),
            "%s",
            "CMakeLists.txt");
        (void)snprintf(entry.name, sizeof(entry.name), "%s", "CMakeLists.txt");
        assert(UmiUiExplorerDescribeFileIndexEntry(
            &entry, &presentation) == UMI_STATUS_OK);
        assert(presentation.parentPath[0] == '\0');
        assert(presentation.topLevel[0] == '\0');
        assert(presentation.depth == 0U);
        assert(presentation.workspaceRoot == 1);

        (void)snprintf(
            entry.relative_path,
            sizeof(entry.relative_path),
            "%s",
            "include\\umicom\\panel.h");
        (void)snprintf(entry.name, sizeof(entry.name), "%s", "panel.h");
        assert(UmiUiExplorerDescribeFileIndexEntry(
            &entry, &presentation) == UMI_STATUS_OK);
        assert(strcmp(presentation.parentPath, "include\\umicom") == 0);
        assert(strcmp(presentation.topLevel, "include") == 0);
        assert(presentation.depth == 2U);
    }

    return 0;
}
