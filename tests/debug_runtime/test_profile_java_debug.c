/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_profile_java_debug.c
 *
 * PURPOSE:
 *   Verify the built-in Java Debug Adapter adapter profile contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/debug_runtime/profiles/java_debug.h"

int main(void)
{
    const UmiDebugAdapterProfile *profile = umi_debug_runtime_profile_java_debug();
    assert(profile != NULL);
    assert(profile->id[0] != '\0');
    assert(profile->executable[0] != '\0');
    assert(profile->debugger_kind[0] != '\0');
    assert(profile->enabled == 1);
    return 0;
}
