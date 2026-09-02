/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_safe_area.c
 *
 * PURPOSE:
 *   Validate resolve platform safe-area insets into renderer-neutral logical content bounds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/safe_area.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiRect rect;
    UmiAdaptiveViewport viewport = { 390, 844 };
    UmiAdaptiveInsets insets = { 44, 0, 34, 0 };
    CHECK(umi_adaptive_safe_area_content_rect(viewport, insets, &rect) == UMI_STATUS_OK);
    CHECK(rect.height == 766);
    CHECK(rect.y == 44);
    return 0;
}
