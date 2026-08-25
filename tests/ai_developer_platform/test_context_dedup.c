#include "umicom/ai/developer_platform/context_dedup.h"
int main(void) { uint64_t a=umi_ai_dev_context_dedup_text("abc"),b=umi_ai_dev_context_dedup_text("abd"); if(a==0U||a==b)return 1; if(!umi_ai_dev_context_dedup_same("same","same"))return 2; return 0; }
