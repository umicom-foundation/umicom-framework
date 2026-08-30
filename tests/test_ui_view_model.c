/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_view_model.c
 *
 * PURPOSE:
 *   Verify view-model hierarchy, visibility, enablement and revisioned properties.
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
    UmiUiViewModel *view = NULL;
    UmiUiViewSnapshot snapshot;
    UmiUiValue value;
    char child[UMI_UI_ID_CAPACITY];
    assert(umi_ui_view_model_create("studio.root", "workbench.root", UMI_UI_ROLE_WORKBENCH, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_set_parent(view, "studio.shell") == UMI_STATUS_OK);
    assert(umi_ui_view_model_add_child(view, "studio.editor") == UMI_STATUS_OK);
    assert(umi_ui_value_set_boolean(&value, 1) == UMI_STATUS_OK);
    assert(umi_ui_view_model_set_property(view, "active", &value) == UMI_STATUS_OK);
    assert(umi_ui_view_model_set_visible(view, 0) == UMI_STATUS_OK);
    assert(umi_ui_view_model_snapshot(view, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.child_count == 1U && snapshot.visible == 0);
    assert(umi_ui_view_model_child_at(view, 0U, child, sizeof(child)) == UMI_STATUS_OK);
    assert(strcmp(child, "studio.editor") == 0);
    assert(umi_ui_view_model_remove_child(view, child) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    return EXIT_SUCCESS;
}
