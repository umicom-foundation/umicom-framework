/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/action_recovery/main.c
 * PURPOSE: Show how shared controllers report failure without losing state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/launcher.h"
#include "umicom/trading_ui/action_controller.h"
#include <stdio.h>
#include <string.h>

/* A real frontend would update a status label here. The callback tells us
 * that the presentation changed; last_status tells us whether the action succeeded. */
static void ShowTradingFeedback(uint64_t revision, void *userData)
{
    const UmiTradingUiController *controller = userData;
    UmiTradingUiControllerSnapshot view = umi_trading_ui_controller_snapshot(controller);
    (void)revision;
    printf("Trader: %s\n", view.last_message);
}

/* No market connection is created. We deliberately request an environment
 * that the default workstation policy forbids and inspect the unchanged model. */
static int ShowRejectedTradingAction(void)
{
    UmiTradingWorkspace *workspace = NULL;
    UmiTradingUiController controller;
    UmiTradingWorkspaceSnapshot state;
    UmiTradingWorkspaceConfig config = umi_trading_workspace_config_default();
    UmiStatus status = umi_trading_workspace_create(&config, &workspace);
    if (status != UMI_STATUS_OK) return 1;
    status = umi_trading_ui_controller_init(&controller, workspace, NULL);
    if (status == UMI_STATUS_OK) {
        /* controller remains alive until after the handler is detached. */
        umi_trading_ui_controller_set_changed_handler(&controller, ShowTradingFeedback, &controller);
        status = umi_trading_ui_controller_set_environment(&controller, UMI_TRADING_LIVE);
        if (status == UMI_STATUS_PERMISSION_DENIED &&
            umi_trading_workspace_snapshot(workspace, &state) == UMI_STATUS_OK &&
            state.environment == UMI_TRADING_SIMULATION && !state.live_armed) {
            puts("Trader: still in simulation; no order was submitted.");
            status = UMI_STATUS_OK;
        } else status = UMI_STATUS_INVALID_STATE;
        umi_trading_ui_controller_set_changed_handler(&controller, NULL, NULL);
    }
    umi_trading_workspace_destroy(workspace);
    return status == UMI_STATUS_OK ? 0 : 1;
}

typedef struct TrainingProcess {
    int confirmExit;
} TrainingProcess;

/* These two adapter functions are a teaching fixture. They do not start,
 * signal or stop an operating-system process. The launcher itself is real. */
static UmiStatus StartTrainingProcess(void *context,
    const UmiApplicationLaunchPlan *plan, uint64_t *outToken)
{
    (void)context; (void)plan;
    *outToken = 101U;
    return UMI_STATUS_OK;
}
static UmiStatus StopTrainingProcess(void *context, const char *applicationId,
    uint64_t token, uint32_t timeoutMs)
{
    TrainingProcess *process = context;
    (void)applicationId; (void)token; (void)timeoutMs;
    return process->confirmExit ? UMI_STATUS_OK : UMI_STATUS_TIMEOUT;
}

/* A failed stop cannot justify forgetting the process or starting a second
 * copy. Only a later confirmed exit permits the stopped state. */
static int ShowStopRecovery(void)
{
    const char *applicationId = "org.umicom.notes";
    UmiApplicationRuntimeCatalogue *catalogue = NULL;
    UmiApplicationLauncher *launcher = NULL;
    UmiApplicationRuntimeRegistration registration = {0};
    UmiApplicationLauncherAdapter adapter = {0};
    UmiApplicationRuntimeRecord record;
    TrainingProcess process = {0};
    UmiStatus status;
    registration.structure_size = sizeof(registration);
    registration.application_id = applicationId;
    registration.display_name = "Umicom Notes";
    registration.executable_name = "umicom-notes";
    registration.family = UMI_APPLICATION_FAMILY_DEVELOPMENT;
    registration.maturity = UMI_APPLICATION_AVAILABLE;
    registration.entry_kind = UMI_APPLICATION_ENTRY_WORKBENCH;
    registration.installed = true; registration.compatible = true; registration.enabled = true;
    adapter.structure_size = sizeof(adapter);
    adapter.adapter_context = &process;
    adapter.start = StartTrainingProcess;
    adapter.stop = StopTrainingProcess;
    status = umi_application_runtime_catalogue_create(&catalogue);
    if (status != UMI_STATUS_OK) return 1;
    status = umi_application_runtime_catalogue_register(catalogue, &registration);
    if (status == UMI_STATUS_OK)
        status = umi_application_launcher_create(catalogue, NULL, &adapter, &launcher);
    if (status == UMI_STATUS_OK)
        status = umi_application_launcher_request(launcher, applicationId, UMI_APPLICATION_LAUNCH_START);
    if (status == UMI_STATUS_OK) {
        status = umi_application_launcher_request(launcher, applicationId, UMI_APPLICATION_LAUNCH_STOP);
        if (status == UMI_STATUS_TIMEOUT &&
            umi_application_runtime_catalogue_find(catalogue, applicationId, &record) == UMI_STATUS_OK &&
            record.state == UMI_APPLICATION_RUNTIME_ATTENTION && record.process_token == 101U) {
            printf("Desk: %s\n", record.last_error);
            puts("Desk: process 101 is still tracked.");
            process.confirmExit = 1;
            status = umi_application_launcher_request(launcher, applicationId, UMI_APPLICATION_LAUNCH_STOP);
            if (status == UMI_STATUS_OK &&
                umi_application_runtime_catalogue_find(catalogue, applicationId, &record) == UMI_STATUS_OK &&
                !record.running && record.process_token == 0U)
                puts("Desk: exit confirmed; the process is now stopped.");
            else status = UMI_STATUS_INVALID_STATE;
        } else status = UMI_STATUS_INVALID_STATE;
    }
    /* Destroy the borrower before the catalogue and callback context expire. */
    umi_application_launcher_destroy(launcher);
    umi_application_runtime_catalogue_destroy(catalogue);
    return status == UMI_STATUS_OK ? 0 : 1;
}

int main(void)
{
    if (ShowRejectedTradingAction() != 0 || ShowStopRecovery() != 0) {
        fputs("The expected recovery behaviour did not occur.\n", stderr);
        return 1;
    }
    return 0;
}
