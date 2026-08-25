#include "umicom/ai/developer_platform/model_health.h"
int main(void) { UmiAiDevModelHealth v; umi_ai_dev_model_health_init(&v); if (umi_ai_dev_model_health_record(&v, 1) != UMI_STATUS_OK) return 1; if (!umi_ai_dev_model_health_ready(&v)) return 2; if (umi_ai_dev_model_health_health_score(&v) != 100U) return 3; return 0; }
