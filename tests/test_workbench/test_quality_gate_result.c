#include "umicom/test/workbench/quality_gate_result.h"
#include <assert.h>
int main(void){UmiQualityGateResult m;assert(umi_quality_gate_result_init(&m,"quality_gate_result","Quality Gate Result")==UMI_STATUS_OK);assert(umi_quality_gate_result_set_active(&m,true)==UMI_STATUS_OK);assert(umi_quality_gate_result_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_quality_gate_result_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_quality_gate_result_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
