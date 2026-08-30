/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_navigation_history.c
 *
 * PURPOSE:
 *   Verify Back/Forward source navigation.
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
#include "umicom/developer_productivity/navigation_history.h"

int main(void)
{
    UmiDeveloperNavigationHistory history;
    UmiDeveloperProductivityLocation first;
    UmiDeveloperProductivityLocation second;
    UmiDeveloperProductivityLocation result;

    umi_developer_navigation_history_init(&history);
    (void)memset(&first, 0, sizeof(first));
    (void)strcpy(first.uri, "a.c");
    first.line = 1U;
    second = first;
    (void)strcpy(second.uri, "b.c");

    assert(umi_developer_navigation_history_push(
        &history, &first) == UMI_STATUS_OK);
    assert(umi_developer_navigation_history_push(
        &history, &second) == UMI_STATUS_OK);
    assert(umi_developer_navigation_history_back(
        &history, &result) == UMI_STATUS_OK);
    assert(strcmp(result.uri, "a.c") == 0);
    return 0;
}
