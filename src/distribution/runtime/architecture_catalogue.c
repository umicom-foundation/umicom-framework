/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/architecture_catalogue.c
 *
 * PURPOSE:
 *   bounded architecture target registry and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/architecture_catalogue.h"

#include <string.h>
/*
 * Initialise dr architecture catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_architecture_catalogue_init(UmiDrArchitectureCatalogue *catalogue) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue != NULL) *catalogue = (UmiDrArchitectureCatalogue){0}; }
/*
 * Find dr architecture catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiDrArchitectureDescriptor *umi_dr_architecture_catalogue_find(const UmiDrArchitectureCatalogue *catalogue, const char *id) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue==NULL||id==NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<catalogue->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(catalogue->items[i].id,id)==0) return &catalogue->items[i]; return NULL; }
/*
 * Add dr architecture catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_dr_architecture_catalogue_add(UmiDrArchitectureCatalogue *catalogue, const UmiDrArchitectureDescriptor *item) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue==NULL||item==NULL||item->id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_dr_architecture_catalogue_find(catalogue,item->id)!=NULL) return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue->count>=UMI_DR_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED; catalogue->items[catalogue->count++]=*item; return UMI_STATUS_OK; }
