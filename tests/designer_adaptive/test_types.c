/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_types.c
 *
 * PURPOSE:
 *   Validate adaptive designer shared values and helper behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/designer/adaptive/types.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    char text[16];
    CHECK(umi_designer_adaptive_copy_text(text, sizeof text, "tablet") == UMI_STATUS_OK);
    CHECK(strcmp(text, "tablet") == 0);
    CHECK(umi_designer_adaptive_size_in_range(UMI_DESIGN_SIZE_EXPANDED,
                                              UMI_DESIGN_SIZE_MEDIUM,
                                              UMI_DESIGN_SIZE_WIDE));
    return 0;
}
