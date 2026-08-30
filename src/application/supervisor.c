/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/supervisor.c
 *
 * PURPOSE:
 *   Implement the Framework-owned composition root used by every application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/supervisor.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/application/component_catalogue.h"
#include "umicom/application/portfolio.h"
#include "umicom/runtime/capability_registry.h"

struct UmiApplicationSupervisor {
    const UmiApplicationDefinition *definition;
    UmiMasterController *master;
    UmiFederationRouter *federation;
    UmiApplicationContextHub *context;
    UmiApplicationResourceBroker *resources;
    int started;
};

static UmiStatus publish_capability(UmiApplicationSupervisor *supervisor,
                                    const char *capability_id,
                                    void *provider)
{
    UmiCapabilityDescriptor descriptor;
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.capability_id = capability_id;
    descriptor.capability_version = (UmiVersion){0U, 5U, 0U};
    descriptor.provider_module_id = "org.umicom.framework.application";
    descriptor.provider = provider;
    descriptor.flags = UMI_CAPABILITY_SINGLETON | UMI_CAPABILITY_EXTERNAL;
    return umi_capability_registry_register(
        umi_master_controller_capabilities(supervisor->master), &descriptor);
}

UmiStatus umi_application_supervisor_create(
    const UmiApplicationSupervisorConfig *config,
    UmiApplicationSupervisor **out_supervisor)
{
    UmiApplicationSupervisor *supervisor;
    UmiMasterControllerConfig master_config;
    UmiStatus status;
    if (config == NULL || config->application_id == NULL ||
        out_supervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_supervisor = NULL;
    supervisor = (UmiApplicationSupervisor *)calloc(1U, sizeof(*supervisor));
    if (supervisor == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    supervisor->definition = umi_application_portfolio_find(
        config->application_id);
    if (supervisor->definition == NULL) {
        free(supervisor);
        return UMI_STATUS_NOT_FOUND;
    }
    (void)memset(&master_config, 0, sizeof(master_config));
    master_config.application_name = supervisor->definition->display_name;
    master_config.diagnostic_sink = config->diagnostic_sink;
    master_config.diagnostic_user_data = config->diagnostic_user_data;
    status = umi_master_controller_create(&master_config, &supervisor->master);
    if (status == UMI_STATUS_OK)
        status = umi_federation_router_create(&supervisor->federation);
    if (status == UMI_STATUS_OK)
        status = umi_application_context_hub_create(&supervisor->context);
    if (status == UMI_STATUS_OK)
        status = umi_application_resource_broker_create(&supervisor->resources);
    if (status == UMI_STATUS_OK)
        status = umi_federation_router_register_application(
            supervisor->federation, supervisor->definition);
    if (status == UMI_STATUS_OK)
        status = publish_capability(supervisor, "umicom.application",
                                    supervisor);
    if (status == UMI_STATUS_OK)
        status = publish_capability(supervisor,
                                    "umicom.application.federation",
                                    supervisor->federation);
    if (status == UMI_STATUS_OK)
        status = publish_capability(supervisor, "umicom.application.context",
                                    supervisor->context);
    if (status == UMI_STATUS_OK)
        status = publish_capability(supervisor,
                                    "umicom.application.components",
                                    supervisor);
    if (status == UMI_STATUS_OK)
        status = publish_capability(supervisor,
                                    "umicom.application.resources",
                                    supervisor->resources);
    if (status != UMI_STATUS_OK) {
        umi_application_supervisor_destroy(supervisor);
        return status;
    }
    *out_supervisor = supervisor;
    return UMI_STATUS_OK;
}

void umi_application_supervisor_destroy(UmiApplicationSupervisor *supervisor)
{
    if (supervisor == NULL) return;
    if (supervisor->started) (void)umi_application_supervisor_stop(supervisor);
    umi_master_controller_destroy(supervisor->master);
    umi_application_context_hub_destroy(supervisor->context);
    umi_application_resource_broker_destroy(supervisor->resources);
    umi_federation_router_destroy(supervisor->federation);
    free(supervisor);
}

UmiStatus umi_application_supervisor_attach(
    UmiApplicationSupervisor *supervisor,
    const UmiApplicationDefinition *application)
{
    if (supervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_federation_router_register_application(
        supervisor->federation, application);
}

UmiStatus umi_application_supervisor_start(
    UmiApplicationSupervisor *supervisor)
{
    UmiStatus status;
    if (supervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (supervisor->started) return UMI_STATUS_INVALID_STATE;
    status = umi_master_controller_start(supervisor->master);
    if (status == UMI_STATUS_OK) supervisor->started = 1;
    return status;
}

UmiStatus umi_application_supervisor_stop(
    UmiApplicationSupervisor *supervisor)
{
    UmiStatus status;
    if (supervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!supervisor->started) return UMI_STATUS_INVALID_STATE;
    status = umi_master_controller_stop(supervisor->master);
    if (status == UMI_STATUS_OK) supervisor->started = 0;
    return status;
}

const UmiApplicationDefinition *umi_application_supervisor_definition(
    const UmiApplicationSupervisor *supervisor)
{
    return supervisor != NULL ? supervisor->definition : NULL;
}

UmiMasterController *umi_application_supervisor_master(
    UmiApplicationSupervisor *supervisor)
{
    return supervisor != NULL ? supervisor->master : NULL;
}

UmiFederationRouter *umi_application_supervisor_federation(
    UmiApplicationSupervisor *supervisor)
{
    return supervisor != NULL ? supervisor->federation : NULL;
}

UmiApplicationContextHub *umi_application_supervisor_context(
    UmiApplicationSupervisor *supervisor)
{
    return supervisor != NULL ? supervisor->context : NULL;
}

UmiApplicationResourceBroker *umi_application_supervisor_resources(
    UmiApplicationSupervisor *supervisor)
{
    return supervisor != NULL ? supervisor->resources : NULL;
}
