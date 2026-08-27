#include "umicom/test/workbench/coverage_delta_model.h"
#include <assert.h>
int main(void){UmiCoverageDeltaModel m;assert(umi_coverage_delta_model_init(&m,"coverage_delta_model","Coverage Delta Model")==UMI_STATUS_OK);assert(umi_coverage_delta_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_coverage_delta_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_coverage_delta_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_coverage_delta_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
