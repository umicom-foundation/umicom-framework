/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/patch.c
 *
 * PURPOSE:
 *   Define one semantic DOM mutation with target, attribute/value and revision metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/patch.h"
#include "../../base/value_construction_internal.h"

#include <string.h>
/*
 * Initialise native web patch from caller-provided values so later operations receive a
 * known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_native_web_patch_init(UmiNativeWebPatch *patch,UmiNativeWebPatchKind kind,const char *target_id,const char *name,const char *value,uint64_t revision){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(patch==NULL||target_id==NULL||target_id[0]=='\0'||kind<UMI_NATIVE_WEB_PATCH_SET_TEXT||kind>UMI_NATIVE_WEB_PATCH_FOCUS)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(patch,0,sizeof(*patch));patch->kind=kind;s=umi_native_web_copy_text(patch->target_id,sizeof(patch->target_id),target_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(name!=NULL){s=umi_native_web_copy_text(patch->name,sizeof(patch->name),name);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(value!=NULL){s=umi_native_web_copy_text(patch->value,sizeof(patch->value),value);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}patch->revision=revision;return UMI_STATUS_OK;}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_native_web_patch_init_build_value(UmiNativeWebPatch *patch,UmiNativeWebPatchKind kind,const char *target_id,const char *name,const char *value,uint64_t revision){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(patch==NULL||target_id==NULL||target_id[0]=='\0'||kind<UMI_NATIVE_WEB_PATCH_SET_TEXT||kind>UMI_NATIVE_WEB_PATCH_FOCUS)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(patch,0,sizeof(*patch));patch->kind=kind;s=umi_native_web_copy_text(patch->target_id,sizeof(patch->target_id),target_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(name!=NULL){s=umi_native_web_copy_text(patch->name,sizeof(patch->name),name);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(value!=NULL){s=umi_native_web_copy_text(patch->value,sizeof(patch->value),value);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}patch->revision=revision;return UMI_STATUS_OK;}

UMI_DEFINE_STAGED_VALUE_INIT(umi_native_web_patch_init, UmiNativeWebPatch, patch,
    (UmiNativeWebPatch *patch, UmiNativeWebPatchKind kind, const char *target_id, const char *name, const char *value, uint64_t revision),
    umi_native_web_patch_init_build_value(candidate, kind, target_id, name, value, revision))

/*
 * Provide the native web patch same slot operation used by this module and its client
 * applications.
 */
bool umi_native_web_patch_same_slot(const UmiNativeWebPatch *left,const UmiNativeWebPatch *right){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(left==NULL||right==NULL)return false;return left->kind==right->kind&&strcmp(left->target_id,right->target_id)==0&&strcmp(left->name,right->name)==0;}

