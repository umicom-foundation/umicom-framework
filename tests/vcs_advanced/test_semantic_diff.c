/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_semantic_diff.c
 *
 * PURPOSE:
 *   Validate estimate structural similarity after whitespace and identifier-boundary normalization.
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
#include "umicom/vcs/advanced/semantic_diff.h"
int main(void){UmiVcsAdvancedSemanticDiff d;
if(umi_vcs_advanced_semantic_diff_compare("int Foo = 1;","int foo=1;",&d)!=UMI_STATUS_OK)return 1;
if(d.similarity_percent!=100U||!d.equivalent)return 2;
    return 0;
}
