/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_workspace_catalogue.c
 *
 * PURPOSE:
 *   Exercise workspace catalogue behaviour and invariants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/workspace_catalogue.h"
int main(void) {
    UmiUiMosaicWorkspaceCatalogue value;
    umi_ui_mosaic_workspace_catalogue_init(&value);
    CHECK(umi_ui_mosaic_workspace_catalogue_set(&value, "perspective.workspace_catalogue", "Workspace Catalogue", "layout.default", UMI_UI_MOSAIC_APP_TMS) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_workspace_catalogue_validate(&value) == UMI_STATUS_OK);
    return 0;
}
