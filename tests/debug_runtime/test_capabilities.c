/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_capabilities.c
 *
 * PURPOSE:
 *   Verify DAP initialize capabilities map to existing debugger capability bits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/debug_runtime/capabilities.h"

int main(void)
{
    UmiDebugRuntimeCapabilities capabilities = {0};
    uint64_t bits;

    capabilities.supports_read_memory_request = 1;
    capabilities.supports_disassemble_request = 1;
    capabilities.supports_step_back = 1;

    bits = umi_debug_runtime_capability_bits(&capabilities);
    assert((bits & UMI_DEBUG_CAP_READ_MEMORY) != 0U);
    assert((bits & UMI_DEBUG_CAP_DISASSEMBLE) != 0U);
    assert((bits & UMI_DEBUG_CAP_STEP_BACK) != 0U);
    assert((bits & UMI_DEBUG_CAP_REVERSE_CONTINUE) != 0U);
    return 0;
}
