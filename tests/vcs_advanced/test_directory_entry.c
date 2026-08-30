/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_directory_entry.c
 *
 * PURPOSE:
 *   Validate describe one filesystem entry for deterministic directory comparison.
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
#include "umicom/vcs/advanced/directory_entry.h"

int main(void)
{
    UmiVcsAdvancedDirectoryEntry value;
    umi_vcs_advanced_directory_entry_init(&value);
    if (umi_vcs_advanced_directory_entry_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_directory_entry_set(&value, "src/a.c", 10U, 123U, 0) != UMI_STATUS_OK) return 2;
    return 0;
}
