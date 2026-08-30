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

int main(void){UmiDesignGaugeSpec s;if(umi_design_gauge_spec_init(&s,0.0,100.0,72.0,80.0,95.0)!=UMI_STATUS_OK)return 1;return s.value==72.0?0:2;}
