/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/action_recovery/test_launcher.c
 * PURPOSE: Verify launch/stop failures retain accurate process ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/launcher.h"
#include <stdio.h>
#include <string.h>
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define APP_ID "org.umicom.studio"
typedef struct LaunchFixture {
    UmiApplicationRuntimeCatalogue *catalogue;
    size_t starts;
    size_t stops;
    UmiStatus stopResult;
    int callbackAction;
} LaunchFixture;

/* Controlled adapters report results but never launch or kill a real process. */
static UmiStatus Start(void *context, const UmiApplicationLaunchPlan *plan, uint64_t *token)
{
    LaunchFixture *fixture = context;
    (void)plan;
    fixture->starts += 1U;
    *token = 100U + fixture->starts;
    return UMI_STATUS_OK;
}
static UmiStatus Stop(void *context, const char *applicationId, uint64_t token, uint32_t timeout)
{
    LaunchFixture *fixture = context;
    UmiStatus status = UMI_STATUS_OK;
    (void)token; (void)timeout;
    fixture->stops += 1U;
    if (fixture->callbackAction == 1 || fixture->callbackAction == 2) {
        status = umi_application_runtime_catalogue_mark_exit(fixture->catalogue, applicationId,
            7, "Process owner reported exit 7.");
        if (status == UMI_STATUS_OK && fixture->callbackAction == 2)
            status = umi_application_runtime_catalogue_set_process(fixture->catalogue, applicationId, 555U);
    } else if (fixture->callbackAction == 3) {
        status = umi_application_runtime_catalogue_set_presence(fixture->catalogue, applicationId, true, true, false);
    }
    return status == UMI_STATUS_OK ? fixture->stopResult : status;
}

int main(int argc, char **argv)
{
    LaunchFixture fixture = {0};
    UmiApplicationLauncher *launcher = NULL;
    UmiApplicationRuntimeRegistration registration = {0};
    UmiApplicationLauncherAdapter adapter = {0};
    UmiApplicationRuntimeRecord before, after;
    UmiApplicationRuntimeSnapshot beforeCatalogue, afterCatalogue;
    UmiApplicationLauncherSnapshot launches;
    UmiStatus status;
    const char *mode = argc > 1 ? argv[1] : "stop-failure";
    registration.structure_size = sizeof(registration);
    registration.application_id = APP_ID;
    registration.display_name = "Umicom Studio IDE";
    registration.executable_name = "umicom-studio-ide";
    registration.family = UMI_APPLICATION_FAMILY_DEVELOPMENT;
    registration.maturity = UMI_APPLICATION_AVAILABLE;
    registration.entry_kind = UMI_APPLICATION_ENTRY_WORKBENCH;
    registration.installed = true; registration.compatible = true; registration.enabled = true;
    REQUIRE(umi_application_runtime_catalogue_create(&fixture.catalogue) == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_register(fixture.catalogue, &registration) == UMI_STATUS_OK);
    adapter.structure_size = sizeof(adapter); adapter.adapter_context = &fixture;
    adapter.start = Start; adapter.stop = Stop; /* Missing activate is intentional. */
    REQUIRE(umi_application_launcher_create(fixture.catalogue, NULL, &adapter, &launcher) == UMI_STATUS_OK);
    REQUIRE(umi_application_launcher_request(launcher, APP_ID, UMI_APPLICATION_LAUNCH_START) == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_find(fixture.catalogue, APP_ID, &before) == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_snapshot(fixture.catalogue, &beforeCatalogue) == UMI_STATUS_OK);
    if (strcmp(mode, "message-atomicity") == 0) {
        char tooLong[UMI_APPLICATION_RUNTIME_MESSAGE_CAPACITY + 1U];
        memset(tooLong, 'x', sizeof(tooLong) - 1U); tooLong[sizeof(tooLong) - 1U] = '\0';
        REQUIRE(umi_application_runtime_catalogue_set_state(fixture.catalogue, APP_ID,
            UMI_APPLICATION_RUNTIME_FAILED, tooLong) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(umi_application_runtime_catalogue_find(fixture.catalogue, APP_ID, &after) == UMI_STATUS_OK);
        REQUIRE(umi_application_runtime_catalogue_snapshot(fixture.catalogue, &afterCatalogue) == UMI_STATUS_OK);
        REQUIRE(memcmp(&before, &after, sizeof(before)) == 0);
        REQUIRE(memcmp(&beforeCatalogue, &afterCatalogue, sizeof(beforeCatalogue)) == 0);
    } else if (strcmp(mode, "activation-missing") == 0) {
        REQUIRE(umi_application_launcher_request(launcher, APP_ID, UMI_APPLICATION_LAUNCH_ACTIVATE) == UMI_STATUS_NOT_IMPLEMENTED);
        REQUIRE(umi_application_runtime_catalogue_find(fixture.catalogue, APP_ID, &after) == UMI_STATUS_OK);
        REQUIRE(memcmp(&before, &after, sizeof(before)) == 0);
        REQUIRE(fixture.starts == 1U);
    } else {
        UmiApplicationLaunchAction action = UMI_APPLICATION_LAUNCH_STOP;
        fixture.stopResult = UMI_STATUS_TIMEOUT;
        if (strcmp(mode, "restart-failure") == 0) action = UMI_APPLICATION_LAUNCH_RESTART;
        else if (strcmp(mode, "stop-failure") == 0 || strcmp(mode, "retry") == 0) { }
        else if (strcmp(mode, "exit-during-failure") == 0) fixture.callbackAction = 1;
        else if (strcmp(mode, "replacement-during-failure") == 0) fixture.callbackAction = 2;
        else if (strcmp(mode, "exit-during-success") == 0) { fixture.callbackAction = 1; fixture.stopResult = UMI_STATUS_OK; }
        else if (strcmp(mode, "replacement-during-success") == 0) {
            fixture.callbackAction = 2; fixture.stopResult = UMI_STATUS_OK; action = UMI_APPLICATION_LAUNCH_RESTART;
        } else if (strcmp(mode, "disabled-during-restart") == 0) {
            fixture.callbackAction = 3; fixture.stopResult = UMI_STATUS_OK; action = UMI_APPLICATION_LAUNCH_RESTART;
        } else { fprintf(stderr, "Unknown test case\n"); return 2; }
        status = umi_application_launcher_request(launcher, APP_ID, action);
        REQUIRE(fixture.starts == 1U && fixture.stops == 1U);
        REQUIRE(umi_application_runtime_catalogue_find(fixture.catalogue, APP_ID, &after) == UMI_STATUS_OK);
        if (fixture.stopResult != UMI_STATUS_OK) REQUIRE(status == fixture.stopResult);
        if (fixture.callbackAction == 1) {
            REQUIRE(!after.running && after.process_token == 0U && after.last_exit_code == 7);
            REQUIRE(strcmp(after.last_error, "Process owner reported exit 7.") == 0);
            if (fixture.stopResult == UMI_STATUS_OK) REQUIRE(status == UMI_STATUS_OK);
        } else if (fixture.callbackAction == 2) {
            REQUIRE(after.running && after.process_token == 555U);
            REQUIRE(after.generation == before.generation + 1U);
            if (fixture.stopResult == UMI_STATUS_OK) REQUIRE(status == UMI_STATUS_INVALID_STATE);
        } else if (fixture.callbackAction == 3) {
            REQUIRE(status == UMI_STATUS_UNAVAILABLE && !after.enabled && !after.running);
        } else {
            REQUIRE(after.state == UMI_APPLICATION_RUNTIME_ATTENTION && after.attention);
            REQUIRE(after.running && after.process_token == before.process_token);
            REQUIRE(after.generation == before.generation);
            REQUIRE(after.last_error[0] != '\0');
            REQUIRE(umi_application_launcher_snapshot(launcher, &launches) == UMI_STATUS_OK);
            REQUIRE(launches.last_status == UMI_STATUS_TIMEOUT && launches.failed_request_count == 1U);
            REQUIRE(umi_application_launcher_request(launcher, APP_ID, UMI_APPLICATION_LAUNCH_START) == UMI_STATUS_NOT_IMPLEMENTED);
            REQUIRE(fixture.starts == 1U); /* An uncertain stop cannot permit a duplicate start. */
            if (strcmp(mode, "retry") == 0) {
                fixture.stopResult = UMI_STATUS_OK;
                REQUIRE(umi_application_launcher_request(launcher, APP_ID, UMI_APPLICATION_LAUNCH_STOP) == UMI_STATUS_OK);
                REQUIRE(umi_application_runtime_catalogue_find(fixture.catalogue, APP_ID, &after) == UMI_STATUS_OK);
                REQUIRE(!after.running && after.process_token == 0U);
                REQUIRE(umi_application_launcher_request(launcher, APP_ID, UMI_APPLICATION_LAUNCH_START) == UMI_STATUS_OK);
                REQUIRE(fixture.starts == 2U);
            }
        }
    }
    umi_application_launcher_destroy(launcher);
    umi_application_runtime_catalogue_destroy(fixture.catalogue);
    puts(mode);
    return 0;
}
