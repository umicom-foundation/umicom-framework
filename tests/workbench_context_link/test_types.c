/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_types.c
 *
 * PURPOSE:
 *   Verify bounded text, context-kind masks and text conversion.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/workbench_context_link/types.h"

int main(void)
{
    char text[8U];
    assert(umi_workbench_context_link_copy_text(text, sizeof(text), "abc") == UMI_STATUS_OK);
    assert(strcmp(text, "abc") == 0);
    assert(umi_workbench_context_link_copy_text(text, sizeof(text), "12345678") == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_workbench_context_link_kind_allowed(
        umi_workbench_context_link_kind_mask(UMI_CONTEXT_KIND_PROJECT),
        UMI_CONTEXT_KIND_PROJECT));
    assert(strcmp(umi_workbench_context_link_mode_text(
        UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW), "follow") == 0);
    return 0;
}
