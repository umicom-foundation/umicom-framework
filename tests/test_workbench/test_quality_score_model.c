#include "umicom/test/workbench/quality_score_model.h"
#include <assert.h>
int main(void){UmiQualityScoreModel m;assert(umi_quality_score_model_init(&m,"quality_score_model","Quality Score Model")==UMI_STATUS_OK);assert(umi_quality_score_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_quality_score_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_quality_score_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_quality_score_model_valid(&m));assert(m.active);assert(m.item_count==3U);return 0;}
