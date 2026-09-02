/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection_provider/test_identity.c
 *
 * PURPOSE:
 *   Verify deterministic provider selection identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/workbench_selection_provider/identity.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char first[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char second[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    assert(umi_workbench_selection_provider_make_id(
        first, sizeof(first), "project", "umicom", 7U) == UMI_STATUS_OK);
    assert(umi_workbench_selection_provider_make_id(
        second, sizeof(second), "project", "umicom", 7U) == UMI_STATUS_OK);
    assert(strcmp(first, second) == 0);
    assert(strstr(first, "project:umicom:7") != NULL);

    return 0;
}
