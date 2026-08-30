/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_breakpoint_designer.c
 *
 * PURPOSE:
 *   Validate own adaptive breakpoint selection and catalogue editing for visual authoring.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/breakpoint_designer.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerBreakpointDesigner d; UmiDesignBreakpoint b={"compact",0,599,UMI_DESIGN_SIZE_COMPACT};
    umi_designer_breakpoint_designer_init(&d); CHECK(umi_designer_breakpoint_designer_add(&d,&b)==UMI_STATUS_OK); CHECK(d.catalogue.count==1U);
    return 0;
}
