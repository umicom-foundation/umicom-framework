/* Umicom Framework | Integration designer fabric | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_INTEGRATION_DESIGNER_FABRIC_H
#define UMICOM_INTEGRATION_DESIGNER_FABRIC_H
#include "umicom/integration/connection_profile.h"
#include "umicom/integration/schema_catalogue.h"
#include "umicom/integration/api_contract.h"
#include "umicom/integration/field_mapping.h"
#include "umicom/integration/workflow.h"
#include "umicom/integration/run_history.h"
typedef struct UmiIntegrationDesignerFabric {
    UmiIntegrationConnectionCatalogue connections;
    UmiIntegrationSchemaCatalogue schemas;
    UmiIntegrationApiContract contracts[UMI_INTEGRATION_DESIGNER_MAX_PROFILES];
    size_t contract_count;
    UmiIntegrationFieldMapping mappings[UMI_INTEGRATION_DESIGNER_MAX_MAPPINGS];
    size_t mapping_count;
    UmiIntegrationWorkflow workflows[UMI_INTEGRATION_DESIGNER_MAX_WORKFLOWS];
    size_t workflow_count;
    UmiIntegrationRunHistory runs;
    uint64_t revision;
} UmiIntegrationDesignerFabric;
typedef struct UmiIntegrationDesignerSnapshot {
    size_t connections;
    size_t schemas;
    size_t contracts;
    size_t mappings;
    size_t workflows;
    UmiIntegrationRunMetrics runs;
    uint64_t revision;
} UmiIntegrationDesignerSnapshot;
void umi_integration_designer_fabric_init(UmiIntegrationDesignerFabric *fabric);
UmiStatus umi_integration_designer_fabric_add_contract(UmiIntegrationDesignerFabric *fabric,const UmiIntegrationApiContract *contract);
UmiStatus umi_integration_designer_fabric_add_mapping(UmiIntegrationDesignerFabric *fabric,const UmiIntegrationFieldMapping *mapping);
UmiStatus umi_integration_designer_fabric_add_workflow(UmiIntegrationDesignerFabric *fabric,const UmiIntegrationWorkflow *workflow);
const UmiIntegrationWorkflow *umi_integration_designer_fabric_find_workflow(const UmiIntegrationDesignerFabric *fabric,const char *id);
UmiStatus umi_integration_designer_fabric_record_run(UmiIntegrationDesignerFabric *fabric,const UmiIntegrationSimulation *simulation);
void umi_integration_designer_fabric_snapshot(const UmiIntegrationDesignerFabric *fabric,UmiIntegrationDesignerSnapshot *out_snapshot);
#endif
