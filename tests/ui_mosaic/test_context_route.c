/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_context_route.c
 *
 * PURPOSE:
 *   Exercise context route behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/context_route.h"
int main(void) {
    UmiUiMosaicContextRoute value;
    umi_ui_mosaic_context_route_init(&value);
    CHECK(umi_ui_mosaic_context_route_set(&value, "link.green", "instrument", "trader.chart") == UMI_STATUS_OK);
    value.colour_index = 2U;
    CHECK(umi_ui_mosaic_context_route_validate(&value) == UMI_STATUS_OK);
    return 0;
}
