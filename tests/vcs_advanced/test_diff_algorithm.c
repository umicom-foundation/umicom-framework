/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_diff_algorithm.c
 *
 * PURPOSE:
 *   Validate choose comparison algorithms deterministically from content size, mode and binary state.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/diff_algorithm.h"
int main(void){if(umi_vcs_advanced_diff_algorithm_select(10U,10U,0,0,0)!=UMI_VCS_DIFF_ALGORITHM_MYERS)return 1;
if(umi_vcs_advanced_diff_algorithm_select(1U,1U,1,0,0)!=UMI_VCS_DIFF_ALGORITHM_BINARY)return 2;
    return 0;
}
