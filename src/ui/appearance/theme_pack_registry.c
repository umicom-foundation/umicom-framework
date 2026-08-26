/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/theme_pack_registry.c
 *
 * PURPOSE:
 *   Catalogue versioned theme packs while preserving stable identity and revision evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_pack_registry.h"
#include <string.h>
UmiStatus umi_appearance_theme_pack_registry_init(UmiAppearanceThemePackRegistry *registry) { if(registry==NULL) return UMI_STATUS_INVALID_ARGUMENT; memset(registry,0,sizeof *registry); return UMI_STATUS_OK; }
UmiStatus umi_appearance_theme_pack_registry_upsert(UmiAppearanceThemePackRegistry *registry, const UmiAppearanceThemePack *item) { size_t i; if(registry==NULL||item==NULL||!umi_appearance_id_valid(item->pack_id)) return UMI_STATUS_INVALID_ARGUMENT; for(i=0;i<registry->count;i++) if(strcmp(registry->items[i].pack_id,item->pack_id)==0) { registry->items[i]=*item; registry->revision++; return UMI_STATUS_OK; } if(registry->count>=UMI_APPEARANCE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED; registry->items[registry->count++]=*item; registry->revision++; return UMI_STATUS_OK; }
const UmiAppearanceThemePack *umi_appearance_theme_pack_registry_find(const UmiAppearanceThemePackRegistry *registry, const char *id) { size_t i; if(registry==NULL||id==NULL) return NULL; for(i=0;i<registry->count;i++) if(strcmp(registry->items[i].pack_id,id)==0) return &registry->items[i]; return NULL; }
size_t umi_appearance_theme_pack_registry_count(const UmiAppearanceThemePackRegistry *registry) { return registry==NULL?0U:registry->count; }
