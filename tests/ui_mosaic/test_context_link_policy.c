/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_context_link_policy.c
 *
 * PURPOSE:
 *   Exercise context link policy behaviour and invariants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/context_link_policy.h"
int main(void) {
    UmiUiMosaicContextLinkPolicy value;
    umi_ui_mosaic_context_link_policy_init(&value);
    CHECK(umi_ui_mosaic_context_link_policy_set(&value, "link.green", "instrument", "trader.chart") == UMI_STATUS_OK);
    value.colour_index = 2U;
    CHECK(umi_ui_mosaic_context_link_policy_validate(&value) == UMI_STATUS_OK);
    return 0;
}
