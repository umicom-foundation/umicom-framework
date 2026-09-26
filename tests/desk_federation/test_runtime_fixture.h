/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desk_federation/test_runtime_fixture.h
 *
 * PURPOSE:
 *   Use the actual Desk runtime with deliberately inert process callbacks.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESK_FEDERATION_TEST_RUNTIME_FIXTURE_H
#define UMICOM_DESK_FEDERATION_TEST_RUNTIME_FIXTURE_H
#include "umicom/desktop/federation/runtime_bridge.h"
#include "umicom/desktop/desk_runtime.h"
#include <string.h>
#include <stdio.h>
#define REQUIRE(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); return 1; } } while (0)
/* These callbacks are test doubles. They do not create a process, manipulate
 * another window, invoke a broker or touch an application's business state. */
static UmiStatus FixtureStart(void *context, const UmiApplicationLaunchPlan *plan, uint64_t *token)
{
    size_t *calls = context; (void)plan; ++*calls; *token = 0U;
    return UMI_STATUS_UNAVAILABLE;
}
static UmiStatus FixtureActivate(void *context, const char *id, uint64_t token)
{
    size_t *calls = context; (void)id; (void)token; ++*calls;
    return UMI_STATUS_NOT_IMPLEMENTED;
}
static UmiStatus FixtureStop(void *context, const char *id, uint64_t token, uint32_t timeout)
{
    size_t *calls = context; (void)id; (void)token; (void)timeout; ++*calls;
    return UMI_STATUS_NOT_IMPLEMENTED;
}
static UmiStatus FixtureRuntime(size_t *calls, UmiDeskRuntime **outRuntime)
{
    UmiDeskRuntimeConfig config = umi_desk_runtime_config_default();
    UmiApplicationLauncherAdapter adapter = {0};
    /* Seeding the canonical portfolio exercises real registration metadata;
     * no test claims installation discovery or launch readiness. */
    adapter.structure_size = sizeof adapter;
    adapter.adapter_context = calls;
    adapter.start = FixtureStart; adapter.activate = FixtureActivate; adapter.stop = FixtureStop;
    return umi_desk_runtime_create(NULL, &config, &adapter, outRuntime);
}
#endif
