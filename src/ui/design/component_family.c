/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/component_family.c
 *
 * PURPOSE:
 *   Group semantic component identifiers into discoverable families such as input, navigation and analytics.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/component_family.h"

#include <string.h>
UmiStatus umi_design_component_family_init(UmiDesignComponentFamily *family,const char *id){if(family==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(family,0,sizeof *family);return umi_design_copy_text(family->id,sizeof family->id,id);}
int umi_design_component_family_contains(const UmiDesignComponentFamily *family,const char *component_id){size_t i;if(family==NULL||component_id==NULL)return 0;for(i=0U;i<family->count;++i)if(strcmp(family->component_ids[i],component_id)==0)return 1;return 0;}
UmiStatus umi_design_component_family_add(UmiDesignComponentFamily *family,const char *component_id){UmiStatus s;if(family==NULL||component_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_design_component_family_contains(family,component_id))return UMI_STATUS_ALREADY_EXISTS;if(family->count>=32U)return UMI_STATUS_CAPACITY_EXCEEDED;s=umi_design_copy_text(family->component_ids[family->count],sizeof family->component_ids[family->count],component_id);if(s!=UMI_STATUS_OK)return s;++family->count;return UMI_STATUS_OK;}
