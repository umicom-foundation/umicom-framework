/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_profile_cppvsdbg.c
 *
 * PURPOSE:
 *   Verify the built-in Visual Studio C++ Debug Adapter adapter profile contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/debug_runtime/profiles/cppvsdbg.h"

int main(void)
{
    const UmiDebugAdapterProfile *profile = umi_debug_runtime_profile_cppvsdbg();
    assert(profile != NULL);
    assert(profile->id[0] != '\0');
    assert(profile->executable[0] != '\0');
    assert(profile->debugger_kind[0] != '\0');
    assert(profile->enabled == 1);
    return 0;
}
