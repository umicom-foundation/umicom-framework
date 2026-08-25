/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/bundle_catalogue.c
 *
 * PURPOSE:
 *   bounded bundle inventory with duplicate rejection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/bundle_catalogue.h"

#include <string.h>
void umi_dr_bundle_catalogue_init(UmiDrBundleCatalogue *catalogue) { if (catalogue != NULL) *catalogue = (UmiDrBundleCatalogue){0}; }
const UmiDrApplicationBundle *umi_dr_bundle_catalogue_find(const UmiDrBundleCatalogue *catalogue, const char *id) { size_t i; if (catalogue==NULL||id==NULL) return NULL; for(i=0U;i<catalogue->count;++i) if(strcmp(catalogue->items[i].id,id)==0) return &catalogue->items[i]; return NULL; }
UmiStatus umi_dr_bundle_catalogue_add(UmiDrBundleCatalogue *catalogue, const UmiDrApplicationBundle *item) { if(catalogue==NULL||item==NULL||item->id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT; if(umi_dr_bundle_catalogue_find(catalogue,item->id)!=NULL) return UMI_STATUS_ALREADY_EXISTS; if(catalogue->count>=UMI_DR_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED; catalogue->items[catalogue->count++]=*item; return UMI_STATUS_OK; }
