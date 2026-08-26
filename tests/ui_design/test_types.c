/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_types.c
 *
 * PURPOSE:
 *   Verify shared design-system utilities and bounded metadata handling.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/types.h"

#include <string.h>
int main(void)
{
    char text[8]; double value=0.0;
    if (umi_design_copy_text(text,sizeof text,"panel") != UMI_STATUS_OK) return 1;
    if (strcmp(text,"panel") != 0) return 2;
    if (umi_design_copy_text(text,4U,"panel") != UMI_STATUS_CAPACITY_EXCEEDED) return 3;
    if (umi_design_clamp(12.0,0.0,10.0,&value) != UMI_STATUS_OK || value != 10.0) return 4;
    return umi_design_hash_text("Umicom") != 0U ? 0 : 5;
}
