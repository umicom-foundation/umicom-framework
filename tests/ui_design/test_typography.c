/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_typography.c
 *
 * PURPOSE:
 *   Verify typography family, size, weight and line-height validation.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/typography.h"

int main(void) { UmiDesignTypography t; if(umi_design_typography_init(&t,"Sans",14.0,500U,1.4)!=UMI_STATUS_OK)return 1; return umi_design_typography_valid(&t)?0:2; }
