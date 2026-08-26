/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_table_adaptation.c
 *
 * PURPOSE:
 *   Validate describe authored table-column priorities and resolve compact preview column budgets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/table_adaptation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerTableAdaptation t=umi_designer_table_adaptation_resolve(12U,UMI_DESIGN_SIZE_MEDIUM);
    CHECK(t.preferred_columns==6U&&t.allow_horizontal_scroll);
    return 0;
}
