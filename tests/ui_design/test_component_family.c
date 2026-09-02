/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_component_family.c
 *
 * PURPOSE:
 *   Verify component-family uniqueness and membership.
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

#include "umicom/ui/design/component_family.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignComponentFamily f;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_component_family_init(&f,"inputs")!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_component_family_add(&f,"text-input")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_component_family_add(&f,"text-input")!=UMI_STATUS_ALREADY_EXISTS)return 3;return umi_design_component_family_contains(&f,"text-input")?0:4;}
