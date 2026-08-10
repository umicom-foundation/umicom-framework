/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_vcs_history.c
 *
 * PURPOSE:
 *   Verify bounded commit-history parsing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/vcs/history.h"

int main(void)
{
    UmiVcsHistory *history = NULL;
    size_t parsed = 0U;
    const char output[] =
        "abc123\x1f" "1700000000\x1f" "Sammy Hegab\x1f"
        "sammy@example.com\x1f" "feat: first\n"
        "def456\x1f" "1700000001\x1f" "Umicom\x1f"
        "umicom@example.com\x1f" "fix: second\n";
    assert(umi_vcs_history_create(&history) == UMI_STATUS_OK);
    assert(umi_vcs_history_parse(history, output, &parsed) == UMI_STATUS_OK);
    assert(parsed == 2U);
    assert(strcmp(umi_vcs_history_at(history, 0U)->commit_id, "abc123") == 0);
    umi_vcs_history_destroy(history);
    return 0;
}
