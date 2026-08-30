/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_issue_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository issue boundary semantics.
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
#include "umicom/repository/issue.h"

int main(void)
{
    UmiRepositoryIssueList *list = calloc(1U, sizeof(*list)); size_t n;
    assert(list != NULL); umi_repository_issue_list_init(list);
    for (n=0U; n<UMI_REPOSITORY_CONTROL_ISSUE_CAPACITY; ++n)
     assert(umi_repository_issue_add(list, UMI_REPOSITORY_CONTROL_INFO, "i", "", "x") == UMI_STATUS_OK);
    assert(umi_repository_issue_add(list, UMI_REPOSITORY_CONTROL_INFO, "i", "", "x") == UMI_STATUS_CAPACITY_EXCEEDED);
    free(list);
    return 0;
}
