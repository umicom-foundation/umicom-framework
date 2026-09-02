/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/framework_demo/main.c
 *
 * PURPOSE:
 *   Demonstrate creation of the Master Controller, registration of one Slave
 *   Controller, capability validation, lifecycle execution, and event output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <stdio.h>

/* Provide the print diagnostic operation used by this module and its client applications. */
static void print_diagnostic(const UmiDiagnostic *diagnostic, void *user_data)
{
    (void)user_data;
    (void)printf("[%s] %s: %s\n",
                 umi_diagnostic_severity_text(diagnostic->severity),
                 diagnostic->source,
                 diagnostic->message);
}

/* Provide the sample start operation used by this module and its client applications. */
static UmiStatus sample_start(UmiModuleContext *context)
{
    return umi_event_bus_publish(context->events,
                                 "framework.sample.started",
                                 "ready",
                                 1U);
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    static const char *provided[] = {"umicom.example.sample", NULL};
    static const char *required[] = {"umicom.messaging.events", NULL};
    UmiMasterController *master = NULL;
    UmiMasterControllerConfig config = {
        "Umicom Framework Demo", print_diagnostic, NULL
    };
    UmiModuleDescriptor sample = {
        .structure_size = sizeof(UmiModuleDescriptor),
        .abi_version = UMICOM_FRAMEWORK_ABI_VERSION,
        .module_id = "umicom.sample",
        .display_name = "Sample Slave Controller",
        .module_version = {0U, 5U, 0U},
        .kind = UMI_MODULE_SERVICE,
        .provided_capabilities = provided,
        .required_capabilities = required,
        .optional_capabilities = NULL,
        .requested_permissions = NULL,
        .module_state = NULL,
        .lifecycle = {
            .configure = NULL,
            .initialise = NULL,
            .start = sample_start,
            .quiesce = NULL,
            .stop = NULL,
            .destroy = NULL
        }
    };

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_master_controller_create(&config, &master) != UMI_STATUS_OK)
        return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_master_controller_register(master, &sample) != UMI_STATUS_OK)
        return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_master_controller_start(master) != UMI_STATUS_OK)
        return 1;
    (void)printf("Framework version: %s\n", UMICOM_FRAMEWORK_VERSION_STRING);
    (void)printf("Application: %s\n",
                 umi_master_controller_application_name(master));
    (void)printf("Modules: %zu\n",
                 umi_master_controller_module_count(master));
    (void)printf("Capabilities: %zu\n",
                 umi_capability_registry_count(
                     umi_master_controller_capabilities(master)));
    (void)printf("Events: %llu\n",
                 (unsigned long long)umi_event_bus_last_sequence(
                     umi_master_controller_events(master)));
    (void)umi_master_controller_stop(master);
    umi_master_controller_destroy(master);
    return 0;
}
