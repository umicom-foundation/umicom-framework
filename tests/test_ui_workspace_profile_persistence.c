/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_ui_workspace_profile_persistence.c
 *
 * PURPOSE:
 *   Verify lossless, versioned persistence of custom workspace layouts and
 *   strict rejection of malformed layout records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiUiWorkspaceProfileSnapshot source = {0};
    UmiUiWorkspaceProfileSnapshot decoded;
    char encoded[UMI_UI_WORKSPACE_PROFILE_TEXT_CAPACITY];

    (void)snprintf(source.profile_id, sizeof(source.profile_id), "%s",
                   "custom-review-layout");
    (void)snprintf(source.label, sizeof(source.label), "%s",
                   "Review | Layout 100%%");
    (void)snprintf(source.description, sizeof(source.description), "%s",
                   "Saved left, right and bottom workspace geometry");
    (void)snprintf(source.icon_name, sizeof(source.icon_name), "%s",
                   "view-grid-symbolic");
    source.sidebar_visible = 1;
    source.auxiliary_sidebar_visible = 1;
    source.bottom_panel_visible = 1;
    source.sidebar_size = 286;
    source.auxiliary_sidebar_size = 404;
    source.bottom_panel_size = 292;
    source.order = 1010;
    source.active = 1;
    source.locked = 1;
    source.editor_split_mode = UMI_UI_EDITOR_SPLIT_ROWS;
    source.editor_split_ratio = 6300;
    source.pane_count = 2U;
    (void)snprintf(source.panes[0].pane_id,
                   sizeof(source.panes[0].pane_id), "%s", "studio.explorer");
    source.panes[0].placement = UMI_UI_PLACEMENT_LEFT;
    source.panes[0].order = 10;
    source.panes[0].visible = 1;
    (void)snprintf(source.panes[1].pane_id,
                   sizeof(source.panes[1].pane_id), "%s", "studio.output");
    source.panes[1].placement = UMI_UI_PLACEMENT_BOTTOM;
    source.panes[1].order = 20;
    source.panes[1].visible = 1;

    assert(umi_ui_workspace_profile_encode(&source, encoded,
                                           sizeof(encoded)) ==
           UMI_STATUS_OK);
    assert(strstr(encoded, "%7C") != NULL);
    assert(strstr(encoded, "%25") != NULL);
    assert(umi_ui_workspace_profile_decode(encoded, &decoded) ==
           UMI_STATUS_OK);
    assert(strcmp(decoded.profile_id, source.profile_id) == 0);
    assert(strcmp(decoded.label, source.label) == 0);
    assert(decoded.sidebar_size == source.sidebar_size);
    assert(decoded.auxiliary_sidebar_size == source.auxiliary_sidebar_size);
    assert(decoded.bottom_panel_size == source.bottom_panel_size);
    assert(decoded.locked && decoded.active);
    assert(decoded.editor_split_mode == UMI_UI_EDITOR_SPLIT_ROWS);
    assert(decoded.editor_split_ratio == 6300);
    assert(decoded.pane_count == 2U);
    assert(strcmp(decoded.panes[1].pane_id, "studio.output") == 0);
    assert(decoded.panes[1].placement == UMI_UI_PLACEMENT_BOTTOM);

    assert(umi_ui_workspace_profile_decode("v2|unsupported", &decoded) ==
           UMI_STATUS_PARSE_ERROR);
    assert(umi_ui_workspace_profile_decode(
               "v1|legacy|Legacy|Description|icon|1|0|1|200|300|240|0|0|0|0|0",
               &decoded) == UMI_STATUS_OK);
    assert(decoded.editor_split_mode == UMI_UI_EDITOR_SPLIT_SINGLE);
    assert(decoded.editor_split_ratio == UMI_UI_EDITOR_SPLIT_RATIO_DEFAULT);
    assert(umi_ui_workspace_profile_decode(
               "v1|custom-bad|Bad|Description|icon|1|1|1|-1|200|200|0|0|0|0|0",
               &decoded) == UMI_STATUS_PARSE_ERROR);
    assert(umi_ui_workspace_profile_decode(
               "v1|custom-nul|Bad%00Name|Description|icon|1|1|1|200|200|200|0|0|0|0|0",
               &decoded) == UMI_STATUS_PARSE_ERROR);
    return EXIT_SUCCESS;
}
