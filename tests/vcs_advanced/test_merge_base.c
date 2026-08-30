/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_merge_base.c
 *
 * PURPOSE:
 *   Validate select merge-base candidates deterministically using generation and timestamp evidence.
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
#include "umicom/vcs/advanced/merge_base.h"
int main(void){UmiVcsAdvancedMergeBase b;
    umi_vcs_advanced_merge_base_init(&b);
if(umi_vcs_advanced_merge_base_add(&b,"old",2U,10U)!=UMI_STATUS_OK)return 1;
if(umi_vcs_advanced_merge_base_add(&b,"new",3U,5U)!=UMI_STATUS_OK)return 2;
if(umi_vcs_advanced_merge_base_select(&b)!=UMI_STATUS_OK)return 3;
if(!umi_vcs_advanced_text_equal(b.selected_oid,"new"))return 4;
    return 0;
}
