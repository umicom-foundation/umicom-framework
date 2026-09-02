/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/appearance_registry.c
 *
 * PURPOSE:
 *   Store bounded resolved appearance profiles for applications, workspaces and previews.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_registry.h"
#include <string.h>
/*
 * Initialise appearance registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_appearance_registry_init(UmiAppearanceAppearanceRegistry *registry) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry==NULL) return UMI_STATUS_INVALID_ARGUMENT; memset(registry,0,sizeof *registry); return UMI_STATUS_OK; }
/*
 * Provide the appearance registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_appearance_registry_upsert(UmiAppearanceAppearanceRegistry *registry, const UmiAppearanceAppearanceProfile *item) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry==NULL||item==NULL||!umi_appearance_id_valid(item->profile_id)) return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<registry->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(registry->items[i].profile_id,item->profile_id)==0) { registry->items[i]=*item; registry->revision++; return UMI_STATUS_OK; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry->count>=UMI_APPEARANCE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED; registry->items[registry->count++]=*item; registry->revision++; return UMI_STATUS_OK; }
/*
 * Find appearance registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiAppearanceAppearanceProfile *umi_appearance_registry_find(const UmiAppearanceAppearanceRegistry *registry, const char *id) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(registry==NULL||id==NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<registry->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(registry->items[i].profile_id,id)==0) return &registry->items[i]; return NULL; }
/*
 * Return the number of records represented by appearance registry without changing their
 * state.
 */
size_t umi_appearance_registry_count(const UmiAppearanceAppearanceRegistry *registry) { return registry==NULL?0U:registry->count; }
