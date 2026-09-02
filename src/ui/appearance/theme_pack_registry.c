/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/theme_pack_registry.c
 *
 * PURPOSE:
 *   Catalogue versioned theme packs while preserving stable identity and revision evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_pack_registry.h"
#include <string.h>
/*
 * Initialise appearance theme pack registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_appearance_theme_pack_registry_init(UmiAppearanceThemePackRegistry *registry) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry==NULL) return UMI_STATUS_INVALID_ARGUMENT; memset(registry,0,sizeof *registry); return UMI_STATUS_OK; }
/*
 * Provide the appearance theme pack registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_appearance_theme_pack_registry_upsert(UmiAppearanceThemePackRegistry *registry, const UmiAppearanceThemePack *item) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry==NULL||item==NULL||!umi_appearance_id_valid(item->pack_id)) return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<registry->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(registry->items[i].pack_id,item->pack_id)==0) { registry->items[i]=*item; registry->revision++; return UMI_STATUS_OK; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry->count>=UMI_APPEARANCE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED; registry->items[registry->count++]=*item; registry->revision++; return UMI_STATUS_OK; }
/*
 * Find appearance theme pack registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiAppearanceThemePack *umi_appearance_theme_pack_registry_find(const UmiAppearanceThemePackRegistry *registry, const char *id) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry==NULL||id==NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<registry->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(registry->items[i].pack_id,id)==0) return &registry->items[i]; return NULL; }
/*
 * Return the number of records represented by appearance theme pack registry without
 * changing their state.
 */
size_t umi_appearance_theme_pack_registry_count(const UmiAppearanceThemePackRegistry *registry) { return registry==NULL?0U:registry->count; }
