#include "umicom/ai/developer_platform/agent_plan.h"
int main(void) { UmiAiDevAgentPlan c; umi_ai_dev_agent_plan_init(&c); if(umi_ai_dev_agent_plan_add(&c,"a")!=UMI_STATUS_OK) return 1; if(umi_ai_dev_agent_plan_add(&c,"a")!=UMI_STATUS_ALREADY_EXISTS) return 2; if(!umi_ai_dev_agent_plan_contains(&c,"a")) return 3; if(umi_ai_dev_agent_plan_remove(&c,"a")!=UMI_STATUS_OK) return 4; return 0; }
