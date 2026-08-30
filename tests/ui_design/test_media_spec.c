/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_media_spec.c
 *
 * PURPOSE:
 *   Verify media presentation semantics distinguish image and timed media.
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

#include "umicom/ui/design/media_spec.h"

int main(void){UmiDesignMediaSpec s;if(umi_design_media_spec_init(&s,UMI_DESIGN_MEDIA_VIDEO,1,0,1,1)!=UMI_STATUS_OK)return 1;return s.loop?0:2;}
