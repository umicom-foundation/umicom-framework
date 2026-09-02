/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_conformance_profile.c
 *
 * PURPOSE:
 *   Verify aggregate semantic design-system conformance evidence and scoring.
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

#include "umicom/ui/design/conformance_profile.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignConformanceProfile p={0};/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_conformance_record(&p,"tokens",1)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_conformance_record(&p,"contrast",1)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_conformance_score(&p)!=1.0)return 3;return umi_design_conformance_passes(&p)?0:4;}
