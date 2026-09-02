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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/component_family.h"

#include <string.h>
/*
 * Initialise design component family from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_design_component_family_init(UmiDesignComponentFamily *family,const char *id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(family==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(family,0,sizeof *family);return umi_design_copy_text(family->id,sizeof family->id,id);}
/*
 * Provide the design component family contains operation used by this module and its
 * client applications.
 */
int umi_design_component_family_contains(const UmiDesignComponentFamily *family,const char *component_id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(family==NULL||component_id==NULL)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<family->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(family->component_ids[i],component_id)==0)return 1;return 0;}
/*
 * Add design component family only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_design_component_family_add(UmiDesignComponentFamily *family,const char *component_id){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(family==NULL||component_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_design_component_family_contains(family,component_id))return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(family->count>=32U)return UMI_STATUS_CAPACITY_EXCEEDED;s=umi_design_copy_text(family->component_ids[family->count],sizeof family->component_ids[family->count],component_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;++family->count;return UMI_STATUS_OK;}
