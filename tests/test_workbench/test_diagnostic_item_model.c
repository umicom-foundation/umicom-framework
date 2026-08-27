#include "umicom/test/workbench/diagnostic_item_model.h"
#include <assert.h>
int main(void){UmiDiagnosticItemModel m;assert(umi_diagnostic_item_model_init(&m,"diagnostic_item_model","Diagnostic Item Model")==UMI_STATUS_OK);assert(umi_diagnostic_item_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_diagnostic_item_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_diagnostic_item_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_diagnostic_item_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
