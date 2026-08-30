/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_font_role.c
 *
 * PURPOSE:
 *   Verify stable names for define semantic font roles for product-neutral typography selection..
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

#include "umicom/ui/design/font_role.h"

#include <string.h>
int main(void) { return strcmp(umi_design_font_role_name(UMI_DESIGN_FONT_MONO),"mono")==0 ? 0 : 1; }
