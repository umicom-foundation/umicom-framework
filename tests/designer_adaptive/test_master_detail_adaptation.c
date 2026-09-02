/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_master_detail_adaptation.c
 *
 * PURPOSE:
 *   Validate resolve master/detail designer composition between split and stacked responsive layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/master_detail_adaptation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    CHECK(umi_designer_master_detail_adaptation_resolve(UMI_DESIGN_SIZE_WIDE,0)==UMI_DESIGNER_MASTER_DETAIL_SPLIT);
    CHECK(umi_designer_master_detail_adaptation_resolve(UMI_DESIGN_SIZE_COMPACT,1)==UMI_DESIGNER_MASTER_DETAIL_OVERLAY);
    return 0;
}
