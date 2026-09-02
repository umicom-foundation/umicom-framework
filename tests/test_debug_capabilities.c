/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_capabilities.c
 *
 * PURPOSE:
 *   Implement the test debug capabilities behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework debugger capability tests.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/debug/capabilities.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugCapabilitySet capabilities;
    const uint64_t advanced =
        UMI_DEBUG_CAP_DATA_BREAKPOINTS |
        UMI_DEBUG_CAP_INSTRUCTION_BREAKPOINTS |
        UMI_DEBUG_CAP_WRITE_MEMORY |
        UMI_DEBUG_CAP_REGISTERS |
        UMI_DEBUG_CAP_REVERSE_CONTINUE;

    umi_debug_capability_set_init(&capabilities);
    assert(umi_debug_capability_set_require(
               &capabilities, UMI_DEBUG_CAP_RESTART |
                                  UMI_DEBUG_CAP_TERMINATE | advanced) ==
           UMI_STATUS_OK);
    assert(umi_debug_capability_set_advertise(
               &capabilities, UMI_DEBUG_CAP_RESTART |
                                  UMI_DEBUG_CAP_TERMINATE | advanced) ==
           UMI_STATUS_OK);
    assert(umi_debug_capability_set_ready(&capabilities));
    assert(umi_debug_capability_set_has(&capabilities,
                                        UMI_DEBUG_CAP_WRITE_MEMORY));
    assert(umi_debug_capability_set_has(&capabilities,
                                        UMI_DEBUG_CAP_REGISTERS));
    return 0;
}
