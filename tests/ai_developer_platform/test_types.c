#include <string.h>
#include "umicom/ai/developer_platform/types.h"
int main(void) {
    if (strcmp(umi_ai_dev_state_text(UMI_AI_DEV_STATE_READY), "ready") != 0) return 1;
    if (umi_ai_dev_percent(25U, 100U) != 25U) return 2;
    if (umi_ai_dev_percent(120U, 100U) != 100U) return 3;
    return 0;
}
