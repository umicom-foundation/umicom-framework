/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_style_sheet.c
 *
 * PURPOSE:
 *   Verify stylesheet exact semantic rule lookup.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/style_sheet.h"

int main(void){UmiDesignStyleSheet s={0};UmiDesignStyleRule r={0},o;r.component_kind=UMI_UI_COMPONENT_BUTTON;r.role=UMI_DESIGN_ROLE_PRIMARY;r.state=UMI_DESIGN_INTERACTION_HOVER;if(umi_design_style_sheet_add(&s,&r)!=UMI_STATUS_OK)return 1;return umi_design_style_sheet_find(&s,UMI_UI_COMPONENT_BUTTON,UMI_DESIGN_ROLE_PRIMARY,UMI_DESIGN_INTERACTION_HOVER,&o)==UMI_STATUS_OK?0:2;}
