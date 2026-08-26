/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_progress_spec.c
 *
 * PURPOSE:
 *   Verify the semantic progress spec contract.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/progress_spec.h"

int main(void){UmiDesignProgressSpec s;if(umi_design_progress_spec_init(&s,0.0,100.0,75.0,0,1)!=UMI_STATUS_OK)return 1;return s.value==75.0?0:2;}
