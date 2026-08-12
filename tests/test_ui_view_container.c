/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_view_container.c
 *
 * PURPOSE:
 *   Exercise one Batch 23 unified-workbench contract through its public C API.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * These tests are intentionally small. They document expected behaviour without starting a graphical application.
 */

#include "umicom/umicom.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiUiViewContainerModel *model = NULL;
    UmiUiViewContainerSnapshot item = {0};
    UmiUiViewContainerSnapshot found = {0};

    assert(umi_ui_view_container_model_create(&model) == UMI_STATUS_OK);

    (void)snprintf(item.container_id, sizeof(item.container_id), "%s", "studio.container.explorer");
    (void)snprintf(item.title, sizeof(item.title), "%s", "Explorer");
    item.placement = UMI_UI_PLACEMENT_LEFT;
    item.visible = 1;
    item.view_count = 2U;
    (void)snprintf(item.view_ids[0], sizeof(item.view_ids[0]), "%s", "studio.view.files");
    (void)snprintf(item.view_ids[1], sizeof(item.view_ids[1]), "%s", "studio.view.open-editors");

    assert(umi_ui_view_container_model_upsert(model, &item) == UMI_STATUS_OK);
    assert(umi_ui_view_container_model_set_active(model, item.container_id) == UMI_STATUS_OK);
    assert(umi_ui_view_container_model_set_active_view(model,
                                                       item.container_id,
                                                       item.view_ids[1]) == UMI_STATUS_OK);
    assert(umi_ui_view_container_model_find(model, item.container_id, &found) == UMI_STATUS_OK);
    assert(found.active == 1);
    assert(strcmp(found.active_view_id, "studio.view.open-editors") == 0);

    umi_ui_view_container_model_destroy(model);
    return 0;
}
