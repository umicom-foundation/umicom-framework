/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_timeline_spec.c
 *
 * PURPOSE:
 *   Verify the semantic timeline spec contract.
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

#include "umicom/ui/design/timeline_spec.h"

int main(void){UmiDesignTimelineSpec s;if(umi_design_timeline_spec_init(&s,24U,8.0,1,1,1)!=UMI_STATUS_OK)return 1;return s.tracks==24U?0:2;}
