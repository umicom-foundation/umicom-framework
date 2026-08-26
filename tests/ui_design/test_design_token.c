/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_design_token.c
 *
 * PURPOSE:
 *   Verify typed design-token construction and validation.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/design_token.h"

int main(void){UmiDesignToken t;UmiDesignRgba c;if(umi_design_color_make(0.2,0.6,0.9,1.0,&c)!=UMI_STATUS_OK)return 1;if(umi_design_token_color(&t,"accent",c)!=UMI_STATUS_OK)return 2;return umi_design_token_valid(&t)?0:3;}
