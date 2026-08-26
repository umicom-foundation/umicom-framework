/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_context_link_member.c
 *
 * PURPOSE:
 *   Exercise context link member behaviour and invariants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/context_link_member.h"
int main(void) {
    UmiUiMosaicContextLinkMember value;
    umi_ui_mosaic_context_link_member_init(&value);
    CHECK(umi_ui_mosaic_context_link_member_set(&value, "link.green", "instrument", "trader.chart") == UMI_STATUS_OK);
    value.colour_index = 2U;
    CHECK(umi_ui_mosaic_context_link_member_validate(&value) == UMI_STATUS_OK);
    return 0;
}
