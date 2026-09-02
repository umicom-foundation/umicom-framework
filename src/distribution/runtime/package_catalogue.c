/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/package_catalogue.c
 *
 * PURPOSE:
 *   bounded package catalogue with name/version/target uniqueness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/package_catalogue.h"

#include <string.h>
/*
 * Initialise dr package catalogue from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_package_catalogue_init(UmiDrPackageCatalogue *catalogue) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue != NULL) *catalogue = (UmiDrPackageCatalogue){0}; }
/*
 * Find dr package catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiDrComponentPackage *umi_dr_package_catalogue_find(const UmiDrPackageCatalogue *catalogue, const char *id) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue==NULL||id==NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<catalogue->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(catalogue->items[i].id,id)==0) return &catalogue->items[i]; return NULL; }
/* Add dr package catalogue only after its inputs and available capacity have been checked. */
UmiStatus umi_dr_package_catalogue_add(UmiDrPackageCatalogue *catalogue, const UmiDrComponentPackage *item) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue==NULL||item==NULL||item->id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_dr_package_catalogue_find(catalogue,item->id)!=NULL) return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue->count>=UMI_DR_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED; catalogue->items[catalogue->count++]=*item; return UMI_STATUS_OK; }
