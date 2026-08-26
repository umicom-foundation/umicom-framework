/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_kpi_spec.c
 *
 * PURPOSE:
 *   Verify the semantic kpi spec contract.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/kpi_spec.h"

int main(void){UmiDesignKpiSpec s;if(umi_design_kpi_spec_init(&s,"Latency",12.0,-2.0,UMI_DESIGN_ROLE_SUCCESS,0)!=UMI_STATUS_OK)return 1;return s.value==12.0?0:2;}
