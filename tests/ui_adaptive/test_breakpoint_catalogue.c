/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_breakpoint_catalogue.c
 *
 * PURPOSE:
 *   Validate catalogue canonical design-system breakpoints and resolve the active viewport range.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/breakpoint_catalogue.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveBreakpointCatalogue catalogue;
    UmiDesignBreakpoint compact = { "compact", 0, 599, UMI_DESIGN_SIZE_COMPACT };
    UmiDesignBreakpoint resolved;
    umi_adaptive_breakpoint_catalogue_init(&catalogue);
    CHECK(umi_adaptive_breakpoint_catalogue_add(&catalogue, &compact) == UMI_STATUS_OK);
    CHECK(umi_adaptive_breakpoint_catalogue_resolve(&catalogue, 390, &resolved) == UMI_STATUS_OK);
    CHECK(resolved.size_class == UMI_DESIGN_SIZE_COMPACT);
    return 0;
}
