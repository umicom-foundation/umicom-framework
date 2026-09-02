/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_gauge_spec.c
 *
 * PURPOSE:
 *   Verify the semantic gauge spec contract.
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

#include "umicom/ui/design/gauge_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignGaugeSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_gauge_spec_init(&s,0.0,100.0,72.0,80.0,95.0)!=UMI_STATUS_OK)return 1;return s.value==72.0?0:2;}
