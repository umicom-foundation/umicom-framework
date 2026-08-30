/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/renderer_registry.c
 *
 * PURPOSE:
 *   bounded registry of frontend renderer profiles with stable identity lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/renderer_registry.h"

#include <string.h>
void umi_fc_renderer_registry_init(UmiFcRendererRegistry *registry){ if(registry!=NULL)*registry=(UmiFcRendererRegistry){0}; }
UmiStatus umi_fc_renderer_registry_upsert(UmiFcRendererRegistry *registry,const UmiFcRendererProfile *profile){ size_t i; if(registry==NULL||umi_fc_renderer_profile_validate(profile)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<registry->count;++i){ if(strcmp(registry->items[i].id,profile->id)==0){ registry->items[i]=*profile; registry->revision++; return UMI_STATUS_OK; }} if(registry->count>=8U)return UMI_STATUS_CAPACITY_EXCEEDED; registry->items[registry->count++]=*profile; registry->revision++; return UMI_STATUS_OK; }
UmiStatus umi_fc_renderer_registry_find(const UmiFcRendererRegistry *registry,const char *id,UmiFcRendererProfile *out_profile){ size_t i; if(registry==NULL||id==NULL||out_profile==NULL)return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<registry->count;++i)if(strcmp(registry->items[i].id,id)==0){*out_profile=registry->items[i];return UMI_STATUS_OK;} return UMI_STATUS_NOT_FOUND; }
