#include "umicom/test/workbench/coverage_summary_model.h"
#include <assert.h>
int main(void){UmiCoverageSummaryModel m;assert(umi_coverage_summary_model_init(&m,"coverage_summary_model","Coverage Summary Model")==UMI_STATUS_OK);assert(umi_coverage_summary_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_coverage_summary_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_coverage_summary_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_coverage_summary_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
