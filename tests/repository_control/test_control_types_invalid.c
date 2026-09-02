/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_control_types_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository control types invalid semantics.
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
#include <string.h>
#include "umicom/repository/control_types.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    assert(strcmp(umi_repository_control_state_text((UmiRepositoryControlState)99), "unknown") == 0);
    return 0;
}
