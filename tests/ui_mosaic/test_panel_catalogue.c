/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_panel_catalogue.c
 *
 * PURPOSE:
 *   Exercise panel catalogue behaviour and invariants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/panel_catalogue.h"
int main(void) {
    UmiUiMosaicPanelCatalogue value;
    umi_ui_mosaic_panel_catalogue_init(&value);
    CHECK(umi_ui_mosaic_panel_catalogue_set(&value, "panel.panel_catalogue", "Panel Catalogue") == UMI_STATUS_OK);
    value.application = UMI_UI_MOSAIC_APP_STUDIO;
    value.priority = 10U;
    CHECK(umi_ui_mosaic_panel_catalogue_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_panel_catalogue_rank(&value, 5U) == 16U);
    return 0;
}
