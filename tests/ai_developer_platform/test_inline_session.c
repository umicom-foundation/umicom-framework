#include "umicom/ai/developer_platform/inline_session.h"
int main(void) { UmiAiDevInlineSession v; umi_ai_dev_inline_session_init(&v); if (umi_ai_dev_inline_session_record(&v, 1) != UMI_STATUS_OK) return 1; if (!umi_ai_dev_inline_session_ready(&v)) return 2; if (umi_ai_dev_inline_session_health_score(&v) != 100U) return 3; return 0; }
