/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_style_rule.c
 *
 * PURPOSE:
 *   Verify semantic style rules store and resolve typed properties.
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

#include "umicom/ui/design/style_rule.h"

int main(void){UmiDesignStyleRule r={0};UmiDesignToken t;UmiDesignStyleValue v,o;r.component_kind=UMI_UI_COMPONENT_BUTTON;r.role=UMI_DESIGN_ROLE_PRIMARY;r.state=UMI_DESIGN_INTERACTION_REST;if(umi_design_token_number(&t,"radius",6.0)!=UMI_STATUS_OK)return 1;if(umi_design_style_value_init(&v,"radius",&t)!=UMI_STATUS_OK)return 2;if(umi_design_style_rule_upsert(&r,&v)!=UMI_STATUS_OK)return 3;if(umi_design_style_rule_find(&r,"radius",&o)!=UMI_STATUS_OK)return 4;return o.value.number==6.0?0:5;}
