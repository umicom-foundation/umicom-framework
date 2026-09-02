/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/install_registry.c
 *
 * PURPOSE:
 *   bounded registry of installed applications and versions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/install_registry.h"

#include <string.h>
/*
 * Initialise dr install registry from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_install_registry_init(UmiDrInstallRegistry *catalogue) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue != NULL) *catalogue = (UmiDrInstallRegistry){0}; }
/*
 * Find dr install registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiDrInstallState *umi_dr_install_registry_find(const UmiDrInstallRegistry *catalogue, const char *id) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue==NULL||id==NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<catalogue->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(catalogue->items[i].id,id)==0) return &catalogue->items[i]; return NULL; }
/* Add dr install registry only after its inputs and available capacity have been checked. */
UmiStatus umi_dr_install_registry_add(UmiDrInstallRegistry *catalogue, const UmiDrInstallState *item) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue==NULL||item==NULL||item->id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_dr_install_registry_find(catalogue,item->id)!=NULL) return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue->count>=UMI_DR_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED; catalogue->items[catalogue->count++]=*item; return UMI_STATUS_OK; }
