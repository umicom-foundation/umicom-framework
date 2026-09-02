/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_profile_health.c
 *
 * PURPOSE:
 *   Verify missing DAP adapters are reported cleanly instead of breaking the
 *   Framework debug service.
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
#include "umicom/debug_runtime/profile_health.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugAdapterProfile profile;
    UmiDebugRuntimeProfileHealth health;

    (void)memset(&profile, 0, sizeof(profile));
    (void)strcpy(profile.id, "missing");
    (void)strcpy(profile.display_name, "Missing");
    (void)strcpy(profile.executable, "umicom-dap-adapter-that-does-not-exist");

    assert(umi_debug_runtime_profile_health_probe(
        &profile, &health) == UMI_STATUS_OK);
    assert(health.available == 0);
    return 0;
}
