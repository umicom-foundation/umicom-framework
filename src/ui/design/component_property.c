/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/component_property.c
 *
 * PURPOSE:
 *   Describe a typed configurable semantic component property for designers and property inspectors.
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

#include "umicom/ui/design/component_property.h"

#include <string.h>
/*
 * Initialise design component property from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_design_component_property_init(UmiDesignComponentProperty *property,const char *id,UmiDesignValueKind value_kind,int required,const char *default_token_id){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(property==NULL||id==NULL||value_kind<UMI_DESIGN_VALUE_COLOR||value_kind>UMI_DESIGN_VALUE_DURATION)return UMI_STATUS_INVALID_ARGUMENT;memset(property,0,sizeof *property);s=umi_design_copy_text(property->id,sizeof property->id,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;property->value_kind=value_kind;property->required=required?1:0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(default_token_id!=NULL)return umi_design_copy_text(property->default_token_id,sizeof property->default_token_id,default_token_id);return UMI_STATUS_OK;}
