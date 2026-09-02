/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_types.c
 * PURPOSE: Verify stable Web workbench enum and bounded-text helpers.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/web/workbench/types.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char text[16U];
    assert(umi_web_workbench_copy_text(text, sizeof(text), "api") == UMI_STATUS_OK);
    assert(strcmp(text, "api") == 0);
    assert(umi_web_workbench_copy_text(text, 3U, "api") == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(strcmp(umi_web_workbench_phase_text(UMI_WEB_WORKBENCH_COMPLETED), "completed") == 0);
    assert(strcmp(umi_web_workbench_auth_kind_text(UMI_WEB_WORKBENCH_AUTH_BEARER), "bearer") == 0);
    assert(strcmp(umi_web_workbench_cloud_provider_text(UMI_WEB_WORKBENCH_CLOUD_AWS), "aws") == 0);
    return 0;
}
