/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/renderer_registry.c
 *
 * PURPOSE:
 *   Register and resolve semantic-contract web renderers without application-specific switch statements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/renderer_registry.h"

#include <string.h>
void umi_native_web_renderer_registry_init(UmiNativeWebRendererRegistry *registry){if(registry!=NULL)(void)memset(registry,0,sizeof(*registry));}
UmiStatus umi_native_web_renderer_registry_add(UmiNativeWebRendererRegistry *registry,const UmiNativeWebRendererDescriptor *descriptor){size_t i;if(registry==NULL||umi_native_web_renderer_descriptor_validate(descriptor)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<registry->count;++i)if(strcmp(registry->items[i].renderer_id,descriptor->renderer_id)==0||strcmp(registry->items[i].semantic_contract,descriptor->semantic_contract)==0)return UMI_STATUS_ALREADY_EXISTS;if(registry->count>=UMI_NATIVE_WEB_MAX_RENDERERS)return UMI_STATUS_CAPACITY_EXCEEDED;registry->items[registry->count++]=*descriptor;++registry->revision;return UMI_STATUS_OK;}
const UmiNativeWebRendererDescriptor *umi_native_web_renderer_registry_resolve(const UmiNativeWebRendererRegistry *registry,const char *semantic_contract){size_t i;if(registry==NULL||semantic_contract==NULL)return NULL;for(i=0;i<registry->count;++i)if(strcmp(registry->items[i].semantic_contract,semantic_contract)==0)return &registry->items[i];return NULL;}

