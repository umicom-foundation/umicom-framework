/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_application_brand.c
 *
 * PURPOSE:
 *   Verify Framework-owned application brand presets remain semantic and toolkit neutral.
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

#include "umicom/ui/design/application_brand.h"

#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignApplicationBrand b;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_application_brand_preset("trader",&b)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(b.display_name,"Umicom Trader")!=0)return 2;return b.density==UMI_DESIGN_DENSITY_COMPACT?0:3;}
