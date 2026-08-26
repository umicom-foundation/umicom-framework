/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_color_role.c
 *
 * PURPOSE:
 *   Verify stable names for define semantic colour roles independent of product palettes and frontend toolkits..
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/color_role.h"

#include <string.h>
int main(void) { return strcmp(umi_design_color_role_name(UMI_DESIGN_COLOR_BORDER),"border")==0 ? 0 : 1; }
