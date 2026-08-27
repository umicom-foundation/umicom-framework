#include "umicom/test/workbench/diagnostic_source_summary.h"
#include <assert.h>
int main(void){UmiDiagnosticSourceSummary m;assert(umi_diagnostic_source_summary_init(&m,"diagnostic_source_summary","Diagnostic Source Summary")==UMI_STATUS_OK);assert(umi_diagnostic_source_summary_set_active(&m,true)==UMI_STATUS_OK);assert(umi_diagnostic_source_summary_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_diagnostic_source_summary_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_diagnostic_source_summary_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
