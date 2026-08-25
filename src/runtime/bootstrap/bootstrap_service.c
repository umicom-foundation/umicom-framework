/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/bootstrap_service.c
 *
 * PURPOSE:
 *   Coordinate graph validation, starter and auto-configuration planning for application launch.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/bootstrap_service.h"
#include "umicom/runtime/bootstrap/auto_configuration_plan.h"
#include "umicom/runtime/bootstrap/bootstrap_plan.h"
#include "umicom/runtime/bootstrap/bootstrap_stage.h"
#include "umicom/runtime/bootstrap/graph_validator.h"


#include <string.h>
UmiStatus umi_bootstrap_service_prepare(
    UmiBootstrapContext *context,
    const UmiBootstrapServiceGraph *service_graph,
    const UmiBootstrapStarterCatalogue *starters,
    const UmiBootstrapAutoConfigurationCatalogue *auto_configurations,
    const UmiBootstrapPropertySet *environment,
    const UmiBootstrapIdList *features,
    const UmiBootstrapIdList *capabilities,
    UmiBootstrapPlan *out_bootstrap_plan,
    UmiBootstrapAutoConfigurationPlan *out_auto_configuration_plan,
    UmiBootstrapIssueReport *out_issues) {
    static const char *const stage_ids[] = {
        "bootstrap.discover", "bootstrap.configure", "bootstrap.resolve",
        "bootstrap.create", "bootstrap.start", "bootstrap.ready"
    };
    size_t i;
    UmiStatus status;
    (void)starters;
    if (context == NULL || service_graph == NULL || auto_configurations == NULL ||
        out_bootstrap_plan == NULL || out_auto_configuration_plan == NULL ||
        out_issues == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_bootstrap_graph_validate(service_graph, out_issues);
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_auto_configuration_plan_build(auto_configurations,
        &context->properties, environment, features, capabilities,
        context->platform_id, out_auto_configuration_plan);
    if (status != UMI_STATUS_OK) return status;
    umi_bootstrap_plan_init(out_bootstrap_plan);
    for (i = 0U; i < sizeof(stage_ids) / sizeof(stage_ids[0]); ++i) {
        UmiBootstrapStage stage;
        status = umi_bootstrap_stage_init(&stage, stage_ids[i], (int32_t)(i * 10U));
        if (status != UMI_STATUS_OK) return status;
        status = umi_bootstrap_plan_add_stage(out_bootstrap_plan, &stage);
        if (status != UMI_STATUS_OK) return status;
    }
    context->resolved_service_count = service_graph->node_count;
    return UMI_STATUS_OK;
}
