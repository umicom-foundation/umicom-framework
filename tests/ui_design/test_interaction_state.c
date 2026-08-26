/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_interaction_state.c
 *
 * PURPOSE:
 *   Verify stable names for define mutually exclusive interaction states used for component style resolution..
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/interaction_state.h"

#include <string.h>
int main(void) { return strcmp(umi_design_interaction_state_name(UMI_DESIGN_INTERACTION_INVALID),"invalid")==0 ? 0 : 1; }
