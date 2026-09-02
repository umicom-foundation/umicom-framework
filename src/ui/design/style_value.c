/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/style_value.c
 *
 * PURPOSE:
 *   Represent a concrete style property as a typed design token value.
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

#include "umicom/ui/design/style_value.h"

#include <string.h>
/*
 * Initialise design style value from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_style_value_init(UmiDesignStyleValue *style_value,const char *property_id,const UmiDesignToken *value){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(style_value==NULL||property_id==NULL||!umi_design_token_valid(value))return UMI_STATUS_INVALID_ARGUMENT;memset(style_value,0,sizeof *style_value);s=umi_design_copy_text(style_value->property_id,sizeof style_value->property_id,property_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;style_value->value=*value;return UMI_STATUS_OK;}
