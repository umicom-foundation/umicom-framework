/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_issue_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository issue invalid semantics.
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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRepositoryIssueList *list = calloc(1U, sizeof(*list));
    assert(list != NULL); umi_repository_issue_list_init(list);
    assert(umi_repository_issue_add(list, (UmiRepositoryControlSeverity)99,
     "x", "", "bad") == UMI_STATUS_INVALID_ARGUMENT);
    free(list);
    return 0;
}
