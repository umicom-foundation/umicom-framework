/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_inspector_spec.c
 *
 * PURPOSE:
 *   Verify the semantic inspector spec contract.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/inspector_spec.h"

int main(void){UmiDesignInspectorSpec s;if(umi_design_inspector_spec_init(&s,8U,250U,1,1)!=UMI_STATUS_OK)return 1;return s.searchable?0:2;}
