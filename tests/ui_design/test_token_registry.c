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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/token_registry.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignTokenRegistry r={0};UmiDesignTokenSet s,o;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_token_set_init(&s,"dark")!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_token_registry_set(&r,UMI_DESIGN_THEME_DARK,&s)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_token_registry_get(&r,UMI_DESIGN_THEME_DARK,&o)!=UMI_STATUS_OK)return 3;return o.id[0]=='d'?0:4;}
