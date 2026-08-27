#include "umicom/test/workbench/coverage_region_model.h"
#include <assert.h>
int main(void){UmiCoverageRegionModel m;assert(umi_coverage_region_model_init(&m,"coverage_region_model","Coverage Region Model")==UMI_STATUS_OK);assert(umi_coverage_region_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_coverage_region_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_coverage_region_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_coverage_region_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
