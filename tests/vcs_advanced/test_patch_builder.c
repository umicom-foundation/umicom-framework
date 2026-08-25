/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_patch_builder.c
 *
 * PURPOSE:
 *   Validate build bounded unified patch text incrementally for export and partial staging.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/patch_builder.h"
int main(void){UmiVcsAdvancedPatchBuilder b;
    umi_vcs_advanced_patch_builder_init(&b);
if(umi_vcs_advanced_patch_builder_begin_file(&b,"a.c","a.c")!=UMI_STATUS_OK)return 1;
if(umi_vcs_advanced_patch_builder_add_hunk(&b,"@@ -1 +1 @@\n-a\n+b\n")!=UMI_STATUS_OK)return 2;
if(b.file_count!=1U||b.hunk_count!=1U||b.length==0U)return 3;
    return 0;
}
