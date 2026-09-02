/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_selection.c
 *
 * PURPOSE:
 *   Verify multi-selection and a stable primary component are independent from frontend selection widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This test exercises the public contract rather than relying on private state,
 * making the behaviour useful as an executable example for new developers.
 */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesignerSelection selection={0};
    assert(umi_designer_selection_set_primary(&selection,"editor")==UMI_STATUS_OK);
    assert(umi_designer_selection_add(&selection,"output")==UMI_STATUS_OK);
    assert(selection.count==2U);
    assert(strcmp(selection.primary,"editor")==0);
    assert(umi_designer_selection_contains(&selection,"output"));
    return EXIT_SUCCESS;
}
