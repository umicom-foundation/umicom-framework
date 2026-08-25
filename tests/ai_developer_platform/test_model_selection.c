#include "umicom/ai/developer_platform/model_selection.h"
int main(void) { UmiAiDevModelSelectionScore a={80U,60U,100U,0U}, b={40U,40U,40U,0U}; if (umi_ai_dev_model_selection_compute(&a) <= umi_ai_dev_model_selection_compute(&b)) return 1; if (!umi_ai_dev_model_selection_prefer(&a,&b)) return 2; if (umi_ai_dev_model_selection_weighted(100U,0U,50U)!=50U) return 3; return 0; }
