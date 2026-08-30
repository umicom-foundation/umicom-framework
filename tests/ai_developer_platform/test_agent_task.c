/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_agent_task.c
 *
 * PURPOSE:
 *   Implement the test agent task behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/agent_task.h"
int main(void) { UmiAiDevAgentTask v; umi_ai_dev_agent_task_init(&v); if (umi_ai_dev_agent_task_record(&v, 1) != UMI_STATUS_OK) return 1; if (!umi_ai_dev_agent_task_ready(&v)) return 2; if (umi_ai_dev_agent_task_health_score(&v) != 100U) return 3; return 0; }
