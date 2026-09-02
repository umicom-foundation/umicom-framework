/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/renderer_registry.c
 *
 * PURPOSE:
 *   Register and resolve semantic-contract web renderers without application-specific switch statements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/renderer_registry.h"

#include <string.h>
/*
 * Initialise native web renderer registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_native_web_renderer_registry_init(UmiNativeWebRendererRegistry *registry){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry!=NULL)(void)memset(registry,0,sizeof(*registry));}
/*
 * Add native web renderer registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_native_web_renderer_registry_add(UmiNativeWebRendererRegistry *registry,const UmiNativeWebRendererDescriptor *descriptor){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry==NULL||umi_native_web_renderer_descriptor_validate(descriptor)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<registry->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(registry->items[i].renderer_id,descriptor->renderer_id)==0||strcmp(registry->items[i].semantic_contract,descriptor->semantic_contract)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry->count>=UMI_NATIVE_WEB_MAX_RENDERERS)return UMI_STATUS_CAPACITY_EXCEEDED;registry->items[registry->count++]=*descriptor;++registry->revision;return UMI_STATUS_OK;}
/*
 * Provide the native web renderer registry resolve operation used by this module and its
 * client applications.
 */
const UmiNativeWebRendererDescriptor *umi_native_web_renderer_registry_resolve(const UmiNativeWebRendererRegistry *registry,const char *semantic_contract){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry==NULL||semantic_contract==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<registry->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(registry->items[i].semantic_contract,semantic_contract)==0)return &registry->items[i];return NULL;}

