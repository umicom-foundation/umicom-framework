/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_context_link_member.c
 *
 * PURPOSE:
 *   Exercise context link member behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/context_link_member.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicContextLinkMember value;
    umi_ui_mosaic_context_link_member_init(&value);
    CHECK(umi_ui_mosaic_context_link_member_set(&value, "link.green", "instrument", "trader.chart") == UMI_STATUS_OK);
    value.colour_index = 2U;
    CHECK(umi_ui_mosaic_context_link_member_validate(&value) == UMI_STATUS_OK);
    return 0;
}
