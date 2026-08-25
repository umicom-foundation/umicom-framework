/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_repository_state.c
 *
 * PURPOSE:
 *   Validate aggregate branch/upstream and in-progress git operation state.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/repository_state.h"

int main(void)
{
    UmiVcsAdvancedRepositoryState value;
    umi_vcs_advanced_repository_state_init(&value);
    if (umi_vcs_advanced_repository_state_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_copy_text(value.head_oid, sizeof(value.head_oid), "abc") != UMI_STATUS_OK) return 2;
    value.ahead = 1U; value.behind = 2U;
    if (umi_vcs_advanced_repository_state_validate(&value) != UMI_STATUS_OK) return 3;
    if (!umi_vcs_advanced_repository_state_diverged(&value)) return 4;
    return 0;
}
