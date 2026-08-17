/*-----------------------------------------------------------------------------
 * Umicom Framework debugger capability tests.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/debug/capabilities.h"

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
