/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_gitlink_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository gitlink boundary semantics.
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
#include "umicom/repository/gitlink.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRepositoryGitlink link;
    assert(umi_repository_gitlink_parse("160000 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 3\tapplications/studio", &link) == UMI_STATUS_OK);
    assert(link.stage == 3U);
    return 0;
}
