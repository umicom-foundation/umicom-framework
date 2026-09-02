/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_service_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository service invalid semantics.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include "umicom/repository/service.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRepositoryControlService *s = calloc(1U, sizeof(*s));
    assert(s != NULL); umi_repository_control_service_init(s);
    assert(umi_repository_control_service_load_gitmodules(s,"bad")==UMI_STATUS_PARSE_ERROR);
    assert(s->controller.state==UMI_REPOSITORY_CONTROL_FAILED);
    free(s);
    return 0;
}
