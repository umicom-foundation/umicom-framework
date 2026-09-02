/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_gitlink_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository gitlink invalid semantics.
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
    assert(umi_repository_gitlink_parse("100644 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 0\tframework", &link) == UMI_STATUS_INVALID_STATE);
    assert(umi_repository_gitlink_parse("garbage", &link) == UMI_STATUS_PARSE_ERROR);
    return 0;
}
