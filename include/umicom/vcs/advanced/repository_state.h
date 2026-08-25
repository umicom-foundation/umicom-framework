/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/repository_state.h
 *
 * PURPOSE:
 *   Aggregate branch/upstream and in-progress Git operation state.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_REPOSITORY_STATE_H
#define UMICOM_VCS_ADVANCED_REPOSITORY_STATE_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsAdvancedRepositoryState {
    uint32_t struct_size;
    uint32_t api_version;
    char branch[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    char upstream[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    char head_oid[UMI_VCS_ADVANCED_OID_CAPACITY];
    size_t ahead;
    size_t behind;
    size_t conflicts;
    int detached_head;
    int merge_in_progress;
    int rebase_in_progress;
    int cherry_pick_in_progress;
    int revert_in_progress;
    int bisect_in_progress;
} UmiVcsAdvancedRepositoryState;

void umi_vcs_advanced_repository_state_init(UmiVcsAdvancedRepositoryState *value);
UmiStatus umi_vcs_advanced_repository_state_validate(const UmiVcsAdvancedRepositoryState *value);
int umi_vcs_advanced_repository_state_operation_in_progress(const UmiVcsAdvancedRepositoryState *value);
int umi_vcs_advanced_repository_state_diverged(const UmiVcsAdvancedRepositoryState *value);

#ifdef __cplusplus
}
#endif

#endif
