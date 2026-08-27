#include "umicom/test/workbench/problem_quick_fix_model.h"
#include <assert.h>
int main(void){UmiProblemQuickFixModel m;assert(umi_problem_quick_fix_model_init(&m,"problem_quick_fix_model","Problem Quick Fix Model")==UMI_STATUS_OK);assert(umi_problem_quick_fix_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_problem_quick_fix_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_problem_quick_fix_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_problem_quick_fix_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
