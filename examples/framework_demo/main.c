#include "umicom/umicom.h"

#include <stdio.h>

static void print_diagnostic(const UmiDiagnostic *diagnostic, void *user_data)
{
    (void)user_data;
    printf("[%s] %s: %s\n", umi_diagnostic_severity_text(diagnostic->severity),
           diagnostic->source, diagnostic->message);
}

static UmiStatus sample_start(UmiModuleContext *context)
{
    return umi_event_bus_publish(context->events, "framework.sample.started", "ready", 1U);
}

int main(void)
{
    UmiMasterController *master = 0;
    UmiMasterControllerConfig config = {"Umicom Framework Demo", print_diagnostic, 0};
    UmiModuleDescriptor sample = {
        sizeof(UmiModuleDescriptor), UMICOM_FRAMEWORK_ABI_VERSION,
        "umicom.sample", "Sample Slave Controller", {0, 2, 0},
        UMI_MODULE_SERVICE, 0, 0, 0,
        {0, 0, sample_start, 0, 0, 0}
    };
    if (umi_master_controller_create(&config, &master) != UMI_STATUS_OK) return 1;
    if (umi_master_controller_register(master, &sample) != UMI_STATUS_OK) return 1;
    if (umi_master_controller_start(master) != UMI_STATUS_OK) return 1;
    printf("Framework version: %s\n", UMICOM_FRAMEWORK_VERSION_STRING);
    printf("Application: %s\n", umi_master_controller_application_name(master));
    printf("Modules: %zu\n", umi_master_controller_module_count(master));
    printf("Events: %llu\n", (unsigned long long)umi_event_bus_last_sequence(umi_master_controller_events(master)));
    (void)umi_master_controller_stop(master);
    umi_master_controller_destroy(master);
    return 0;
}
