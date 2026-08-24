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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/repository/gitlink.h"

int main(void)
{
    UmiRepositoryGitlink link;
    assert(umi_repository_gitlink_parse("160000 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 3\tapplications/studio", &link) == UMI_STATUS_OK);
    assert(link.stage == 3U);
    return 0;
}
