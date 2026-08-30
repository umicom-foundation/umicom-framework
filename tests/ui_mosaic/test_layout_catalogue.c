/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_layout_catalogue.c
 *
 * PURPOSE:
 *   Exercise layout catalogue behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/layout_catalogue.h"
int main(void) {
    UmiUiMosaicLayoutCatalogue value;
    umi_ui_mosaic_layout_catalogue_init(&value);
    CHECK(umi_ui_mosaic_layout_catalogue_set(&value, "layout.layout_catalogue", "Layout Catalogue") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_layout_catalogue_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_layout_catalogue_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}
