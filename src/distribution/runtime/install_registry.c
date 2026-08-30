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
void umi_dr_install_registry_init(UmiDrInstallRegistry *catalogue) { if (catalogue != NULL) *catalogue = (UmiDrInstallRegistry){0}; }
const UmiDrInstallState *umi_dr_install_registry_find(const UmiDrInstallRegistry *catalogue, const char *id) { size_t i; if (catalogue==NULL||id==NULL) return NULL; for(i=0U;i<catalogue->count;++i) if(strcmp(catalogue->items[i].id,id)==0) return &catalogue->items[i]; return NULL; }
UmiStatus umi_dr_install_registry_add(UmiDrInstallRegistry *catalogue, const UmiDrInstallState *item) { if(catalogue==NULL||item==NULL||item->id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT; if(umi_dr_install_registry_find(catalogue,item->id)!=NULL) return UMI_STATUS_ALREADY_EXISTS; if(catalogue->count>=UMI_DR_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED; catalogue->items[catalogue->count++]=*item; return UMI_STATUS_OK; }
