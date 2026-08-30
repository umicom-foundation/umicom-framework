/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/surface_catalogue.c
 *
 * PURPOSE:
 *   bounded catalogue of semantic surface requirements shared by all frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/surface_catalogue.h"

#include <string.h>
void umi_fc_surface_catalogue_init(UmiFcSurfaceCatalogue *catalogue){if(catalogue!=NULL)*catalogue=(UmiFcSurfaceCatalogue){0};}
UmiStatus umi_fc_surface_catalogue_add(UmiFcSurfaceCatalogue *catalogue,const UmiFcSurfaceRequirement *requirement){ size_t i; if(catalogue==NULL||requirement==NULL||requirement->surface_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<catalogue->count;++i)if(strcmp(catalogue->items[i].surface_id,requirement->surface_id)==0)return UMI_STATUS_ALREADY_EXISTS; if(catalogue->count>=UMI_FC_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; catalogue->items[catalogue->count++]=*requirement; return UMI_STATUS_OK; }
UmiStatus umi_fc_surface_catalogue_find(const UmiFcSurfaceCatalogue *catalogue,const char *surface_id,UmiFcSurfaceRequirement *out_requirement){size_t i;if(catalogue==NULL||surface_id==NULL||out_requirement==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<catalogue->count;++i)if(strcmp(catalogue->items[i].surface_id,surface_id)==0){*out_requirement=catalogue->items[i];return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
