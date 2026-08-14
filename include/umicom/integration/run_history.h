/* Umicom Framework | Integration run history | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_INTEGRATION_RUN_HISTORY_H
#define UMICOM_INTEGRATION_RUN_HISTORY_H
#include "umicom/integration/simulator.h"
typedef struct UmiIntegrationRunRecord {
    char run_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char workflow_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    UmiIntegrationDesignerRunStatus status;
    size_t step_count;
    unsigned attempted_requests;
    uint64_t ordinal;
} UmiIntegrationRunRecord;
typedef struct UmiIntegrationRunHistory {
    UmiIntegrationRunRecord items[UMI_INTEGRATION_DESIGNER_MAX_RUNS];
    size_t count;
    uint64_t next_ordinal;
} UmiIntegrationRunHistory;
typedef struct UmiIntegrationRunMetrics {
    size_t total;
    size_t succeeded;
    size_t failed;
    size_t cancelled;
    unsigned attempted_requests;
} UmiIntegrationRunMetrics;
UmiStatus umi_integration_run_history_append(UmiIntegrationRunHistory *history,const UmiIntegrationSimulation *simulation);
const UmiIntegrationRunRecord *umi_integration_run_history_find(const UmiIntegrationRunHistory *history,const char *run_id);
void umi_integration_run_history_metrics(const UmiIntegrationRunHistory *history,UmiIntegrationRunMetrics *out_metrics);
#endif
