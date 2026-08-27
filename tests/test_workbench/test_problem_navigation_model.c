#include "umicom/test/workbench/problem_navigation_model.h"
#include <assert.h>
int main(void){UmiProblemNavigationModel m;assert(umi_problem_navigation_model_init(&m,"problem_navigation_model","Problem Navigation Model")==UMI_STATUS_OK);assert(umi_problem_navigation_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_problem_navigation_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_problem_navigation_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_problem_navigation_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
