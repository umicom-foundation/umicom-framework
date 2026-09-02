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
/**
 * Represent the vcs advanced patch builder data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedPatchBuilder {
    uint32_t struct_size; uint32_t api_version; char text[UMI_VCS_ADVANCED_PATCH_CAPACITY];
    size_t length,file_count,hunk_count; uint64_t fingerprint;
} UmiVcsAdvancedPatchBuilder;
/**
 * Initialise vcs advanced patch builder from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_patch_builder_init(UmiVcsAdvancedPatchBuilder *builder);
/**
 * Add vcs advanced patch builder only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_vcs_advanced_patch_builder_append(UmiVcsAdvancedPatchBuilder *builder,const char *text);
/**
 * Provide the vcs advanced patch builder begin file operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_patch_builder_begin_file(UmiVcsAdvancedPatchBuilder *builder,const char *left,const char *right);
/**
 * Provide the vcs advanced patch builder add hunk operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_patch_builder_add_hunk(UmiVcsAdvancedPatchBuilder *builder,const char *hunk_text);
/**
 * Provide the vcs advanced patch builder text operation used by this module and its client
 * applications.
 */
const char *umi_vcs_advanced_patch_builder_text(const UmiVcsAdvancedPatchBuilder *builder);
#ifdef __cplusplus
}
#endif
#endif
