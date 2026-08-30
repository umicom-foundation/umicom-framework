/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_vcs_status.c
 *
 * PURPOSE:
 *   Verify Git porcelain status and branch parsing.
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
#include "umicom/vcs/status.h"

int main(void)
{
    UmiVcsChangeList *changes = NULL;
    size_t parsed = 0U;
    assert(umi_vcs_change_list_create(&changes) == UMI_STATUS_OK);
    assert(umi_vcs_status_parse_porcelain("M  staged.c\n"
                                           " M working.c\n"
                                           "?? new.c\n"
                                           "R  old.c -> new-name.c\n",
                                           changes,
                                           &parsed) == UMI_STATUS_OK);
    assert(parsed == 4U);
    assert(umi_vcs_change_list_count(changes) == 4U);
    assert(umi_vcs_change_list_staged_count(changes) == 2U);
    assert(strcmp(umi_vcs_change_list_at(changes, 3U)->original_path, "old.c") == 0);
    umi_vcs_change_list_destroy(changes);
    return 0;
}
