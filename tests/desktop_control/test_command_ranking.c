/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_command_ranking.c
 * PURPOSE: Validate the Framework-owned command ranking contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/command_ranking.h"

int main(void)
{
    UmiDesktopCommandRankingSnapshot value;
    UmiDesktopCommandRankingSnapshot copy;
    umi_desktop_command_ranking_init(&value, "command_ranking.primary");
    assert(umi_desktop_command_ranking_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_command_ranking_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_command_ranking_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_command_ranking_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_command_ranking_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_command_ranking_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_command_ranking_score(10U, 2U, true) == 1012U);
    return 0;
}
