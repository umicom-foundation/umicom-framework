/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_context_link_colour.c
 *
 * PURPOSE:
 *   Exercise context link colour behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/context_link_colour.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicContextLinkColour value;
    umi_ui_mosaic_context_link_colour_init(&value);
    CHECK(umi_ui_mosaic_context_link_colour_set(&value, "link.green", "instrument", "trader.chart") == UMI_STATUS_OK);
    value.colour_index = 2U;
    CHECK(umi_ui_mosaic_context_link_colour_validate(&value) == UMI_STATUS_OK);
    return 0;
}
