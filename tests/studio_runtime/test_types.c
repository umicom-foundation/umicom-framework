/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_types.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/types.h"
int main(void)
{
    assert(strcmp(umi_studio_runtime_surface_kind_text(
        UMI_STUDIO_SURFACE_EDITOR), "editor") == 0);
    assert(strcmp(umi_studio_runtime_selection_kind_text(
        UMI_STUDIO_SELECTION_PROBLEM), "problem") == 0);
    assert(strcmp(umi_studio_runtime_close_decision_text(
        UMI_STUDIO_CLOSE_ALLOW), "allow") == 0);
    return 0;
}

