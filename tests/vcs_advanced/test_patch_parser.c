/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_patch_parser.c
 *
 * PURPOSE:
 *   Validate parse unified-diff metadata and aggregate file/hunk/change counts without applying content.
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
#include "umicom/vcs/advanced/patch_parser.h"
int main(void){UmiVcsAdvancedPatchSummary s;const char *p="diff --git a/a b/a\n--- a/a\n+++ b/a\n@@ -1 +1 @@\n-old\n+new\n";
if(umi_vcs_advanced_patch_parse_summary(p,&s)!=UMI_STATUS_OK)return 1;
if(s.file_count!=1U||s.hunk_count!=1U||s.added_lines!=1U||s.deleted_lines!=1U)return 2;
    return 0;
}
