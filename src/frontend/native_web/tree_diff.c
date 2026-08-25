/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/tree_diff.c
 *
 * PURPOSE:
 *   Compute deterministic keyed element-tree differences as browser patch operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/tree_diff.h"

#include "umicom/frontend/native_web/element_attribute.h"
#include <string.h>
static UmiStatus add_patch(UmiNativeWebPatchBatch *batch,UmiNativeWebPatchKind kind,const char *id,const char *name,const char *value,uint64_t rev){UmiNativeWebPatch p;UmiStatus s=umi_native_web_patch_init(&p,kind,id,name,value,rev);if(s!=UMI_STATUS_OK)return s;return umi_native_web_patch_batch_add(batch,&p);}
UmiStatus umi_native_web_tree_diff(const UmiNativeWebElementTree *previous,const UmiNativeWebElementTree *current,UmiNativeWebPatchBatch *out_batch){size_t i,j;const UmiNativeWebSemanticElement *old;const char *oldv;bool found;UmiStatus s;if(previous==NULL||current==NULL||out_batch==NULL)return UMI_STATUS_INVALID_ARGUMENT;umi_native_web_patch_batch_init(out_batch,current->revision);for(i=0;i<previous->count;++i)if(umi_native_web_element_tree_find(current,previous->elements[i].element_id)==NULL){s=add_patch(out_batch,UMI_NATIVE_WEB_PATCH_REMOVE_ELEMENT,previous->elements[i].element_id,NULL,NULL,current->revision);if(s!=UMI_STATUS_OK)return s;}for(i=0;i<current->count;++i){old=umi_native_web_element_tree_find(previous,current->elements[i].element_id);if(old==NULL){s=add_patch(out_batch,UMI_NATIVE_WEB_PATCH_INSERT_ELEMENT,current->elements[i].element_id,NULL,current->elements[i].text,current->revision);if(s!=UMI_STATUS_OK)return s;continue;}if(strcmp(old->text,current->elements[i].text)!=0){s=add_patch(out_batch,UMI_NATIVE_WEB_PATCH_SET_TEXT,current->elements[i].element_id,NULL,current->elements[i].text,current->revision);if(s!=UMI_STATUS_OK)return s;}for(j=0;j<current->elements[i].attribute_count;++j){oldv=umi_native_web_element_attribute_get(old,current->elements[i].attributes[j].name);if(oldv==NULL||strcmp(oldv,current->elements[i].attributes[j].value)!=0){s=add_patch(out_batch,UMI_NATIVE_WEB_PATCH_SET_ATTRIBUTE,current->elements[i].element_id,current->elements[i].attributes[j].name,current->elements[i].attributes[j].value,current->revision);if(s!=UMI_STATUS_OK)return s;}}for(j=0;j<old->attribute_count;++j){found=umi_native_web_element_attribute_get(&current->elements[i],old->attributes[j].name)!=NULL;if(!found){s=add_patch(out_batch,UMI_NATIVE_WEB_PATCH_REMOVE_ATTRIBUTE,current->elements[i].element_id,old->attributes[j].name,NULL,current->revision);if(s!=UMI_STATUS_OK)return s;}}}return UMI_STATUS_OK;}

