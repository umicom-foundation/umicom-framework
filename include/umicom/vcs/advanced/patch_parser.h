/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/patch_parser.h
 *
 * PURPOSE:
 *   Parse unified-diff metadata and aggregate file/hunk/change counts without applying content.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_PATCH_PARSER_H
#define UMICOM_VCS_ADVANCED_PATCH_PARSER_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsAdvancedPatchSummary {
    uint32_t struct_size; uint32_t api_version; size_t file_count,hunk_count,added_lines,deleted_lines;
    int has_binary; int malformed;
} UmiVcsAdvancedPatchSummary;
UmiStatus umi_vcs_advanced_patch_parse_summary(const char *patch,UmiVcsAdvancedPatchSummary *out_summary);
#ifdef __cplusplus
}
#endif
#endif
