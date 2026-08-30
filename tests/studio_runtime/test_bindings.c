/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_bindings.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime bindings.
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
#include "umicom/studio_runtime/bindings.h"
int main(void)
{
    UmiStudioRuntimeBindings bindings;
    umi_studio_runtime_bindings_init(&bindings);
    assert(bindings.revision == 1U);
    assert(umi_studio_runtime_bindings_validate(&bindings) ==
           UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}

