/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/profile_binding.c
 *
 * PURPOSE:
 *   Implement the profile binding behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Profile Binding | MIT */
#include "umicom/project/workspace/profile_binding.h"
#include "../../base/value_construction_internal.h"
#include "internal.h"
#include <string.h>
/*
 * Initialise project workspace profile binding from caller-provided values so later
 * operations receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_project_workspace_profile_binding_init(UmiProjectWorkspaceProfileBinding *value,const char *owner_id,const char *target_id) {
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||owner_id==NULL||target_id==NULL||owner_id[0]=='\0'||target_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    s=umi_pw_copy(value->owner_id,sizeof(value->owner_id),owner_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s!=UMI_STATUS_OK)return s;
    s=umi_pw_copy(value->target_id,sizeof(value->target_id),target_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s!=UMI_STATUS_OK)return s;
    value->revision=1U;
    value->enabled=true;
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_project_workspace_profile_binding_init_build_value(UmiProjectWorkspaceProfileBinding *value,const char *owner_id,const char *target_id) {
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||owner_id==NULL||target_id==NULL||owner_id[0]=='\0'||target_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    s=umi_pw_copy(value->owner_id,sizeof(value->owner_id),owner_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s!=UMI_STATUS_OK)return s;
    s=umi_pw_copy(value->target_id,sizeof(value->target_id),target_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s!=UMI_STATUS_OK)return s;
    value->revision=1U;
    value->enabled=true;
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_project_workspace_profile_binding_init, UmiProjectWorkspaceProfileBinding, value,
    (UmiProjectWorkspaceProfileBinding *value, const char *owner_id, const char *target_id),
    umi_project_workspace_profile_binding_init_build_value(candidate, owner_id, target_id))

/*
 * Provide the project workspace profile binding retarget operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_profile_binding_retarget(UmiProjectWorkspaceProfileBinding *value,const char *target_id) {
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_pw_copy(value->target_id,sizeof(value->target_id),target_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
}
/*
 * Provide the project workspace profile binding matches operation used by this module and
 * its client applications.
 */
bool umi_project_workspace_profile_binding_matches(const UmiProjectWorkspaceProfileBinding *value,const char *owner_id,const char *target_id) {
    return value!=NULL&&owner_id!=NULL&&target_id!=NULL&&strcmp(value->owner_id,owner_id)==0&&strcmp(value->target_id,target_id)==0;
}
