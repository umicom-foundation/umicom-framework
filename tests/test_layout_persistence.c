/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_layout_persistence.c
 *
 * PURPOSE:
 *   Implement the test layout persistence behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Layout persistence tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/ui/layout_persistence.h"
int main(void)
{
    UmiUiLayoutPersistenceRecord record = {0};
    UmiUiLayoutPersistenceRecord restored;
    UmiUiWorkspaceWindow explorer = {0};
    UmiUiWorkspaceWindow editor = {0};
    UmiUiLayoutPersistenceRecord legacy;
    char encoded[UMI_UI_LAYOUT_ENCODED_CAPACITY];
    char trailing_record[UMI_UI_LAYOUT_ENCODED_CAPACITY];
    (void)snprintf(explorer.window_id, sizeof(explorer.window_id), "explorer");
    (void)snprintf(explorer.title, sizeof(explorer.title), "Explorer");
    (void)snprintf(explorer.tool_id, sizeof(explorer.tool_id), "explorer");
    (void)snprintf(explorer.group_id, sizeof(explorer.group_id), "left");
    (void)snprintf(explorer.stack_id, sizeof(explorer.stack_id), "navigation");
    (void)snprintf(explorer.placement_id, sizeof(explorer.placement_id), "left");
    (void)snprintf(explorer.context_group_id,
                   sizeof(explorer.context_group_id), "project-blue");
    explorer.width = 0.2; explorer.height = 1.0;
    explorer.visible = true; explorer.pinned = true; explorer.resizable = true;
    editor = explorer;
    (void)snprintf(editor.window_id, sizeof(editor.window_id), "editor");
    (void)snprintf(editor.title, sizeof(editor.title), "Editor");
    (void)snprintf(editor.tool_id, sizeof(editor.tool_id), "editor");
    (void)snprintf(editor.group_id, sizeof(editor.group_id), "centre");
    (void)snprintf(editor.stack_id, sizeof(editor.stack_id), "documents");
    (void)snprintf(editor.placement_id, sizeof(editor.placement_id), "centre");
    editor.x = 0.2; editor.width = 0.8; editor.closable = true;
    record.schema_version = UMI_UI_LAYOUT_PERSISTENCE_SCHEMA_VERSION;
    record.saved_at_ns = 100U;
    assert(umi_ui_workspace_layout_init(&record.layout,"develop","Develop") == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_set_locked(&record.layout,false) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_add_window(&record.layout,&explorer) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_add_window(&record.layout,&editor) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_set_locked(&record.layout,true) == UMI_STATUS_OK);
    assert(umi_ui_layout_persistence_encode(&record,encoded,sizeof(encoded)) == UMI_STATUS_OK);
    assert(umi_ui_layout_persistence_decode(encoded,&restored) == UMI_STATUS_OK);
    assert(restored.layout.window_count == 2U);
    assert(strcmp(restored.layout.windows[1].window_id,"editor") == 0);
    assert(strcmp(restored.layout.windows[0].placement_id,"left") == 0);
    assert(strcmp(restored.layout.windows[0].stack_id,"navigation") == 0);
    assert(strcmp(restored.layout.windows[0].context_group_id,
                  "project-blue") == 0);
    assert(restored.layout.windows[0].pinned);
    assert(restored.layout.windows[1].resizable);
    assert(restored.layout.locked);
    /* A record is one complete reviewed unit; undeclared trailing windows or
     * metadata must not be silently ignored by the decoder. */
    (void)snprintf(
        trailing_record,
        sizeof(trailing_record),
        "%sEXTRA\n",
        encoded);
    assert(umi_ui_layout_persistence_decode(
               trailing_record, &restored) == UMI_STATUS_PARSE_ERROR);
    assert(umi_ui_layout_persistence_decode(
        "UMILAYOUT2\t2\t1\tlegacy\tLegacy\t1\t1\t2\n"
        "W\texplorer\tExplorer\texplorer\tleft\t0\t0\t0.2\t1\t1\t0\t0\t1\t1\n",
        &legacy) == UMI_STATUS_OK);
    assert(strcmp(legacy.layout.windows[0].stack_id, "left") == 0);
    assert(legacy.layout.windows[0].resizable);
    return 0;
}
