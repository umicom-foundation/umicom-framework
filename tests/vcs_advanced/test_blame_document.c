/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_blame_document.c
 *
 * PURPOSE:
 *   Validate maintain bounded line provenance for an opened source document.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/blame_document.h"
int main(void)
{
    UmiVcsAdvancedBlameDocument d; UmiVcsAdvancedBlameLine l;
    umi_vcs_advanced_blame_document_init(&d); umi_vcs_advanced_blame_line_init(&l);
    if (umi_vcs_advanced_blame_document_set_identity(&d, "a.c", "HEAD") != UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_blame_line_set(&l, 1U, "abc", "u", "a.c", "x") != UMI_STATUS_OK) return 2;
    if (umi_vcs_advanced_blame_document_add(&d, &l) != UMI_STATUS_OK) return 3;
    if (umi_vcs_advanced_blame_document_line(&d, 1U) == NULL) return 4;
    if (umi_vcs_advanced_blame_document_distinct_commit_count(&d) != 1U) return 5;
    return 0;
}
