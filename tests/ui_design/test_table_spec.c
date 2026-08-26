/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_table_spec.c
 *
 * PURPOSE:
 *   Verify the semantic table spec contract.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/table_spec.h"

int main(void){UmiDesignTableSpec s;if(umi_design_table_spec_init(&s,12U,2U,UMI_DESIGN_DENSITY_COMPACT,1,1,1)!=UMI_STATUS_OK)return 1;return s.frozen_columns==2U?0:2;}
