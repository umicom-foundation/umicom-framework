/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_table_adaptation.c
 *
 * PURPOSE:
 *   Validate describe authored table-column priorities and resolve compact preview column budgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/table_adaptation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesignerTableAdaptation t=umi_designer_table_adaptation_resolve(12U,UMI_DESIGN_SIZE_MEDIUM);
    CHECK(t.preferred_columns==6U&&t.allow_horizontal_scroll);
    return 0;
}
