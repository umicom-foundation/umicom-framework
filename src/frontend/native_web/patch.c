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

#include <string.h>
UmiStatus umi_native_web_patch_init(UmiNativeWebPatch *patch,UmiNativeWebPatchKind kind,const char *target_id,const char *name,const char *value,uint64_t revision){UmiStatus s;if(patch==NULL||target_id==NULL||target_id[0]=='\0'||kind<UMI_NATIVE_WEB_PATCH_SET_TEXT||kind>UMI_NATIVE_WEB_PATCH_FOCUS)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(patch,0,sizeof(*patch));patch->kind=kind;s=umi_native_web_copy_text(patch->target_id,sizeof(patch->target_id),target_id);if(s!=UMI_STATUS_OK)return s;if(name!=NULL){s=umi_native_web_copy_text(patch->name,sizeof(patch->name),name);if(s!=UMI_STATUS_OK)return s;}if(value!=NULL){s=umi_native_web_copy_text(patch->value,sizeof(patch->value),value);if(s!=UMI_STATUS_OK)return s;}patch->revision=revision;return UMI_STATUS_OK;}
bool umi_native_web_patch_same_slot(const UmiNativeWebPatch *left,const UmiNativeWebPatch *right){if(left==NULL||right==NULL)return false;return left->kind==right->kind&&strcmp(left->target_id,right->target_id)==0&&strcmp(left->name,right->name)==0;}

