/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/icon_catalogue.c
 *
 * PURPOSE:
 *   Catalogue reusable semantic icons independently of GTK, Qt or browser asset APIs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/icon_catalogue.h"
#include <string.h>
UmiStatus umi_appearance_icon_catalogue_init(UmiAppearanceIconCatalogue *registry) { if(registry==NULL) return UMI_STATUS_INVALID_ARGUMENT; memset(registry,0,sizeof *registry); return UMI_STATUS_OK; }
UmiStatus umi_appearance_icon_catalogue_upsert(UmiAppearanceIconCatalogue *registry, const UmiAppearanceIconDescriptor *item) { size_t i; if(registry==NULL||item==NULL||!umi_appearance_id_valid(item->icon_id)) return UMI_STATUS_INVALID_ARGUMENT; for(i=0;i<registry->count;i++) if(strcmp(registry->items[i].icon_id,item->icon_id)==0) { registry->items[i]=*item; registry->revision++; return UMI_STATUS_OK; } if(registry->count>=UMI_APPEARANCE_MAX_ICONS) return UMI_STATUS_CAPACITY_EXCEEDED; registry->items[registry->count++]=*item; registry->revision++; return UMI_STATUS_OK; }
const UmiAppearanceIconDescriptor *umi_appearance_icon_catalogue_find(const UmiAppearanceIconCatalogue *registry, const char *id) { size_t i; if(registry==NULL||id==NULL) return NULL; for(i=0;i<registry->count;i++) if(strcmp(registry->items[i].icon_id,id)==0) return &registry->items[i]; return NULL; }
size_t umi_appearance_icon_catalogue_count(const UmiAppearanceIconCatalogue *registry) { return registry==NULL?0U:registry->count; }
