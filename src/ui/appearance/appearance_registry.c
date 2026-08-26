/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/appearance_registry.c
 *
 * PURPOSE:
 *   Store bounded resolved appearance profiles for applications, workspaces and previews.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_registry.h"
#include <string.h>
UmiStatus umi_appearance_registry_init(UmiAppearanceAppearanceRegistry *registry) { if(registry==NULL) return UMI_STATUS_INVALID_ARGUMENT; memset(registry,0,sizeof *registry); return UMI_STATUS_OK; }
UmiStatus umi_appearance_registry_upsert(UmiAppearanceAppearanceRegistry *registry, const UmiAppearanceAppearanceProfile *item) { size_t i; if(registry==NULL||item==NULL||!umi_appearance_id_valid(item->profile_id)) return UMI_STATUS_INVALID_ARGUMENT; for(i=0;i<registry->count;i++) if(strcmp(registry->items[i].profile_id,item->profile_id)==0) { registry->items[i]=*item; registry->revision++; return UMI_STATUS_OK; } if(registry->count>=UMI_APPEARANCE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED; registry->items[registry->count++]=*item; registry->revision++; return UMI_STATUS_OK; }
const UmiAppearanceAppearanceProfile *umi_appearance_registry_find(const UmiAppearanceAppearanceRegistry *registry, const char *id) { size_t i; if(registry==NULL||id==NULL) return NULL; for(i=0;i<registry->count;i++) if(strcmp(registry->items[i].profile_id,id)==0) return &registry->items[i]; return NULL; }
size_t umi_appearance_registry_count(const UmiAppearanceAppearanceRegistry *registry) { return registry==NULL?0U:registry->count; }
