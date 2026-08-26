/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_size_class.c
 *
 * PURPOSE:
 *   Verify deterministic responsive width classification.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/size_class.h"

int main(void){UmiDesignSizeClass c;if(umi_design_size_class_from_width(1200,&c)!=UMI_STATUS_OK)return 1;return c==UMI_DESIGN_SIZE_EXPANDED?0:2;}
