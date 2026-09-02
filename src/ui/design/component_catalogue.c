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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/component_catalogue.h"

#include <string.h>
/*
 * Provide the design component catalogue upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_design_component_catalogue_upsert(UmiDesignComponentCatalogue *catalogue,const UmiDesignComponentDescriptor *descriptor){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue==NULL||!umi_design_component_descriptor_valid(descriptor))return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<catalogue->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(catalogue->items[i].id,descriptor->id)==0){catalogue->items[i]=*descriptor;++catalogue->revision;return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue->count>=UMI_DESIGN_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;catalogue->items[catalogue->count]=*descriptor;++catalogue->count;++catalogue->revision;return UMI_STATUS_OK;}
/*
 * Find design component catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_design_component_catalogue_find(const UmiDesignComponentCatalogue *catalogue,const char *id,UmiDesignComponentDescriptor *out_descriptor){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue==NULL||id==NULL||out_descriptor==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<catalogue->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(catalogue->items[i].id,id)==0){*out_descriptor=catalogue->items[i];return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
