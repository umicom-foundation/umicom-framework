#include "umicom/test/workbench/diagnostic_group_model.h"
#include <assert.h>
int main(void){UmiDiagnosticGroupModel m;assert(umi_diagnostic_group_model_init(&m,"diagnostic_group_model","Diagnostic Group Model")==UMI_STATUS_OK);assert(umi_diagnostic_group_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_diagnostic_group_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_diagnostic_group_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_diagnostic_group_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
