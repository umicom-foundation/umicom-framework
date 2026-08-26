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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/component_property.h"

#include <string.h>
UmiStatus umi_design_component_property_init(UmiDesignComponentProperty *property,const char *id,UmiDesignValueKind value_kind,int required,const char *default_token_id){UmiStatus s;if(property==NULL||id==NULL||value_kind<UMI_DESIGN_VALUE_COLOR||value_kind>UMI_DESIGN_VALUE_DURATION)return UMI_STATUS_INVALID_ARGUMENT;memset(property,0,sizeof *property);s=umi_design_copy_text(property->id,sizeof property->id,id);if(s!=UMI_STATUS_OK)return s;property->value_kind=value_kind;property->required=required?1:0;if(default_token_id!=NULL)return umi_design_copy_text(property->default_token_id,sizeof property->default_token_id,default_token_id);return UMI_STATUS_OK;}
