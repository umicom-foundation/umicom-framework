/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_context_pack.c
 *
 * PURPOSE:
 *   Implement the test context pack behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/context_pack.h"
int main(void) { UmiAiDevContextPack c; umi_ai_dev_context_pack_init(&c); if(umi_ai_dev_context_pack_add(&c,"a")!=UMI_STATUS_OK) return 1; if(umi_ai_dev_context_pack_add(&c,"a")!=UMI_STATUS_ALREADY_EXISTS) return 2; if(!umi_ai_dev_context_pack_contains(&c,"a")) return 3; if(umi_ai_dev_context_pack_remove(&c,"a")!=UMI_STATUS_OK) return 4; return 0; }
