/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_dialog_spec.c
 *
 * PURPOSE:
 *   Verify the semantic dialog spec contract.
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

#include "umicom/ui/design/dialog_spec.h"

int main(void){UmiDesignDialogSpec s;if(umi_design_dialog_spec_init(&s,UMI_DESIGN_SIZE_MEDIUM,2U,1,1)!=UMI_STATUS_OK)return 1;return s.modal?0:2;}
