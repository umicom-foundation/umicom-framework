/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/rebase_step.h
 *
 * PURPOSE:
 *   Describe one interactive rebase instruction while preserving commit identity.
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
#ifndef UMICOM_VCS_ADVANCED_REBASE_STEP_H
#define UMICOM_VCS_ADVANCED_REBASE_STEP_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsAdvancedRebaseStep {
    uint32_t struct_size;
    uint32_t api_version;
    char commit_oid[UMI_VCS_ADVANCED_OID_CAPACITY];
    char subject[UMI_VCS_ADVANCED_TEXT_CAPACITY];
    uint32_t sequence;
    uint32_t action;
} UmiVcsAdvancedRebaseStep;

void umi_vcs_advanced_rebase_step_init(UmiVcsAdvancedRebaseStep *value);
UmiStatus umi_vcs_advanced_rebase_step_validate(const UmiVcsAdvancedRebaseStep *value);
typedef enum UmiVcsAdvancedRebaseAction {
    UMI_VCS_REBASE_PICK = 0,
    UMI_VCS_REBASE_REWORD = 1,
    UMI_VCS_REBASE_EDIT = 2,
    UMI_VCS_REBASE_SQUASH = 3,
    UMI_VCS_REBASE_FIXUP = 4,
    UMI_VCS_REBASE_DROP = 5,
    UMI_VCS_REBASE_EXEC = 6
} UmiVcsAdvancedRebaseAction;
UmiStatus umi_vcs_advanced_rebase_step_set(UmiVcsAdvancedRebaseStep *value,
                                             const char *commit_oid,
                                             const char *subject,
                                             size_t sequence,
                                             UmiVcsAdvancedRebaseAction action);

#ifdef __cplusplus
}
#endif

#endif
