/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_operation_guard.c
 *
 * PURPOSE:
 *   Validate describe preconditions that protect mutating source-control operations.
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
#include "umicom/vcs/advanced/operation_guard.h"

int main(void)
{
    UmiVcsAdvancedOperationGuard value;
    umi_vcs_advanced_operation_guard_init(&value);
    if (umi_vcs_advanced_operation_guard_validate(&value) != UMI_STATUS_OK) return 2;
    if (!umi_vcs_advanced_operation_guard_allows(&value, 1, 0, 1, 0, 0)) return 3;
    if (umi_vcs_advanced_operation_guard_allows(&value, 1, 1, 1, 0, 0)) return 4;
    return 0;
}
