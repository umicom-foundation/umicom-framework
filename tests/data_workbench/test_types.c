/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_workbench/test_types.c
 *
 * PURPOSE:
 *   Verify stable Database workbench lifecycle and schema-change text.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/data/workbench/types.h"

int main(void)
{
    char text[16];
    assert(strcmp(umi_data_workbench_phase_text(
                      UMI_DATA_WORKBENCH_EDITING_SQL), "editing-sql") == 0);
    assert(strcmp(umi_data_schema_change_kind_text(
                      UMI_DATA_SCHEMA_CHANGED), "changed") == 0);
    assert(umi_data_workbench_copy_text(text, sizeof(text), "database") ==
           UMI_STATUS_OK);
    assert(strcmp(text, "database") == 0);
    return 0;
}
