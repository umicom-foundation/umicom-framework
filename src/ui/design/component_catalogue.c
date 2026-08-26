/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/component_catalogue.c
 *
 * PURPOSE:
 *   Maintain a searchable bounded catalogue of reusable semantic component descriptors.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/component_catalogue.h"

#include <string.h>
UmiStatus umi_design_component_catalogue_upsert(UmiDesignComponentCatalogue *catalogue,const UmiDesignComponentDescriptor *descriptor){size_t i;if(catalogue==NULL||!umi_design_component_descriptor_valid(descriptor))return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<catalogue->count;++i)if(strcmp(catalogue->items[i].id,descriptor->id)==0){catalogue->items[i]=*descriptor;++catalogue->revision;return UMI_STATUS_OK;}if(catalogue->count>=UMI_DESIGN_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;catalogue->items[catalogue->count]=*descriptor;++catalogue->count;++catalogue->revision;return UMI_STATUS_OK;}
UmiStatus umi_design_component_catalogue_find(const UmiDesignComponentCatalogue *catalogue,const char *id,UmiDesignComponentDescriptor *out_descriptor){size_t i;if(catalogue==NULL||id==NULL||out_descriptor==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<catalogue->count;++i)if(strcmp(catalogue->items[i].id,id)==0){*out_descriptor=catalogue->items[i];return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
