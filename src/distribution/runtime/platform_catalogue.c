/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/platform_catalogue.c
 *
 * PURPOSE:
 *   bounded registry of supported operating-system targets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/platform_catalogue.h"

#include <string.h>
void umi_dr_platform_catalogue_init(UmiDrPlatformCatalogue *catalogue) { if (catalogue != NULL) *catalogue = (UmiDrPlatformCatalogue){0}; }
const UmiDrPlatformDescriptor *umi_dr_platform_catalogue_find(const UmiDrPlatformCatalogue *catalogue, const char *id) { size_t i; if (catalogue==NULL||id==NULL) return NULL; for(i=0U;i<catalogue->count;++i) if(strcmp(catalogue->items[i].id,id)==0) return &catalogue->items[i]; return NULL; }
UmiStatus umi_dr_platform_catalogue_add(UmiDrPlatformCatalogue *catalogue, const UmiDrPlatformDescriptor *item) { if(catalogue==NULL||item==NULL||item->id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT; if(umi_dr_platform_catalogue_find(catalogue,item->id)!=NULL) return UMI_STATUS_ALREADY_EXISTS; if(catalogue->count>=UMI_DR_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED; catalogue->items[catalogue->count++]=*item; return UMI_STATUS_OK; }
