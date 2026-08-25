/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_moved_block.c
 *
 * PURPOSE:
 *   Validate capture identical or near-identical blocks moved within a document.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/moved_block.h"

int main(void)
{
    UmiVcsAdvancedMovedBlock value;
    umi_vcs_advanced_moved_block_init(&value);
    if (umi_vcs_advanced_moved_block_validate(&value) == UMI_STATUS_OK) return 1;
    value.old_start = 1U; value.new_start = 20U; value.line_count = 5U; value.confidence_percent = 100U;
    if (umi_vcs_advanced_moved_block_validate(&value) != UMI_STATUS_OK) return 2;
    if (!umi_vcs_advanced_moved_block_is_significant(&value, 3U, 90U)) return 3;
    return 0;
}
