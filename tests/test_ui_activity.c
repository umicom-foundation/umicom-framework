/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_activity.c
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

int main(void)
{
    UmiUiActivityModel *model = NULL;
    UmiUiActivitySnapshot item = {0};
    UmiUiActivitySnapshot found = {0};

    assert(umi_ui_activity_model_create(&model) == UMI_STATUS_OK);

    (void)snprintf(item.activity_id, sizeof(item.activity_id), "%s", "studio.activity.explorer");
    (void)snprintf(item.label, sizeof(item.label), "%s", "Explorer");
    (void)snprintf(item.container_id, sizeof(item.container_id), "%s", "studio.container.explorer");
    item.visible = 1;
    item.enabled = 1;
    item.order = 10;

    assert(umi_ui_activity_model_upsert(model, &item) == UMI_STATUS_OK);
    assert(umi_ui_activity_model_set_active(model, item.activity_id) == UMI_STATUS_OK);
    assert(umi_ui_activity_model_find(model, item.activity_id, &found) == UMI_STATUS_OK);
    assert(found.active == 1);
    assert(strcmp(found.container_id, "studio.container.explorer") == 0);

    umi_ui_activity_model_destroy(model);
    return 0;
}
