/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/icon_catalogue.c
 *
 * PURPOSE:
 *   Catalogue reusable semantic icons independently of GTK, Qt or browser asset APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/icon_catalogue.h"
#include <string.h>
/*
 * Initialise appearance icon catalogue from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_appearance_icon_catalogue_init(UmiAppearanceIconCatalogue *registry) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry==NULL) return UMI_STATUS_INVALID_ARGUMENT; memset(registry,0,sizeof *registry); return UMI_STATUS_OK; }
/*
 * Provide the appearance icon catalogue upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_appearance_icon_catalogue_upsert(UmiAppearanceIconCatalogue *registry, const UmiAppearanceIconDescriptor *item) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry==NULL||item==NULL||!umi_appearance_id_valid(item->icon_id)) return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<registry->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(registry->items[i].icon_id,item->icon_id)==0) { registry->items[i]=*item; registry->revision++; return UMI_STATUS_OK; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry->count>=UMI_APPEARANCE_MAX_ICONS) return UMI_STATUS_CAPACITY_EXCEEDED; registry->items[registry->count++]=*item; registry->revision++; return UMI_STATUS_OK; }
/*
 * Find appearance icon catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiAppearanceIconDescriptor *umi_appearance_icon_catalogue_find(const UmiAppearanceIconCatalogue *registry, const char *id) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry==NULL||id==NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<registry->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(registry->items[i].icon_id,id)==0) return &registry->items[i]; return NULL; }
/*
 * Return the number of records represented by appearance icon catalogue without changing
 * their state.
 */
size_t umi_appearance_icon_catalogue_count(const UmiAppearanceIconCatalogue *registry) { return registry==NULL?0U:registry->count; }
