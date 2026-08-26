/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_token_registry.c
 *
 * PURPOSE:
 *   Verify mode-specific design-token registry resolution.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/token_registry.h"

int main(void){UmiDesignTokenRegistry r={0};UmiDesignTokenSet s,o;if(umi_design_token_set_init(&s,"dark")!=UMI_STATUS_OK)return 1;if(umi_design_token_registry_set(&r,UMI_DESIGN_THEME_DARK,&s)!=UMI_STATUS_OK)return 2;if(umi_design_token_registry_get(&r,UMI_DESIGN_THEME_DARK,&o)!=UMI_STATUS_OK)return 3;return o.id[0]=='d'?0:4;}
