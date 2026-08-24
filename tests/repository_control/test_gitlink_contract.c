/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_gitlink_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository gitlink contract semantics.
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
    assert(umi_repository_gitlink_parse("160000 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 0\tframework", &link) == UMI_STATUS_OK);
    assert(link.valid);
    assert(strcmp(link.path, "framework") == 0);
    return 0;
}
