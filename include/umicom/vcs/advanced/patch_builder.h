/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/patch_builder.h
 *
 * PURPOSE:
 *   Build bounded unified patch text incrementally for export and partial staging.
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
#ifndef UMICOM_VCS_ADVANCED_PATCH_BUILDER_H
#define UMICOM_VCS_ADVANCED_PATCH_BUILDER_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsAdvancedPatchBuilder {
    uint32_t struct_size; uint32_t api_version; char text[UMI_VCS_ADVANCED_PATCH_CAPACITY];
    size_t length,file_count,hunk_count; uint64_t fingerprint;
} UmiVcsAdvancedPatchBuilder;
void umi_vcs_advanced_patch_builder_init(UmiVcsAdvancedPatchBuilder *builder);
UmiStatus umi_vcs_advanced_patch_builder_append(UmiVcsAdvancedPatchBuilder *builder,const char *text);
UmiStatus umi_vcs_advanced_patch_builder_begin_file(UmiVcsAdvancedPatchBuilder *builder,const char *left,const char *right);
UmiStatus umi_vcs_advanced_patch_builder_add_hunk(UmiVcsAdvancedPatchBuilder *builder,const char *hunk_text);
const char *umi_vcs_advanced_patch_builder_text(const UmiVcsAdvancedPatchBuilder *builder);
#ifdef __cplusplus
}
#endif
#endif
