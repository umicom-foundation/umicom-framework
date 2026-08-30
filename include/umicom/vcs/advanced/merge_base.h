/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/merge_base.h
 *
 * PURPOSE:
 *   Select merge-base candidates deterministically using generation and timestamp evidence.
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
#ifndef UMICOM_VCS_ADVANCED_MERGE_BASE_H
#define UMICOM_VCS_ADVANCED_MERGE_BASE_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsAdvancedMergeBaseCandidate {
    char oid[UMI_VCS_ADVANCED_OID_CAPACITY]; uint32_t generation; uint64_t timestamp_seconds;
} UmiVcsAdvancedMergeBaseCandidate;
typedef struct UmiVcsAdvancedMergeBase {
    uint32_t struct_size; uint32_t api_version;
    UmiVcsAdvancedMergeBaseCandidate candidates[16]; size_t candidate_count;
    char selected_oid[UMI_VCS_ADVANCED_OID_CAPACITY];
} UmiVcsAdvancedMergeBase;
void umi_vcs_advanced_merge_base_init(UmiVcsAdvancedMergeBase *value);
UmiStatus umi_vcs_advanced_merge_base_add(UmiVcsAdvancedMergeBase *value,const char *oid,uint32_t generation,uint64_t timestamp);
UmiStatus umi_vcs_advanced_merge_base_select(UmiVcsAdvancedMergeBase *value);
#ifdef __cplusplus
}
#endif
#endif
