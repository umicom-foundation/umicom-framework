/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_tree_spec.c
 *
 * PURPOSE:
 *   Verify the semantic tree spec contract.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/tree_spec.h"

int main(void){UmiDesignTreeSpec s;if(umi_design_tree_spec_init(&s,16U,1,1,1)!=UMI_STATUS_OK)return 1;return s.checkboxes?0:2;}
