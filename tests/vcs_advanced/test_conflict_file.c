/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_conflict_file.c
 *
 * PURPOSE:
 *   Validate capture index-stage availability and resolution state for a conflicted path.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/conflict_file.h"

int main(void)
{
    UmiVcsAdvancedConflictFile value;
    umi_vcs_advanced_conflict_file_init(&value);
    if (umi_vcs_advanced_conflict_file_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_conflict_file_set(&value, "src/conflict.c", 1, 1, 1) != UMI_STATUS_OK) return 2;
    return 0;
}
