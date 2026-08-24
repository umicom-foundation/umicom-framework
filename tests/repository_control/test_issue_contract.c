/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_issue_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository issue contract semantics.
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
#include <stdlib.h>
#include "umicom/repository/issue.h"

int main(void)
{
    UmiRepositoryIssueList *list = calloc(1U, sizeof(*list));
    assert(list != NULL); umi_repository_issue_list_init(list);
    assert(umi_repository_issue_add(list, UMI_REPOSITORY_CONTROL_WARNING,
     "repo.head", "framework", "Head differs") == UMI_STATUS_OK);
    assert(umi_repository_issue_count_severity(list, UMI_REPOSITORY_CONTROL_WARNING) == 1U);
    free(list);
    return 0;
}
