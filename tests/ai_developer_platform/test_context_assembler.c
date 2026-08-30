/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_context_assembler.c
 *
 * PURPOSE:
 *   Implement the test context assembler behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/context_assembler.h"
int main(void) { UmiAiDevContextAssembler c; umi_ai_dev_context_assembler_init(&c); if(umi_ai_dev_context_assembler_add(&c,"a")!=UMI_STATUS_OK) return 1; if(umi_ai_dev_context_assembler_add(&c,"a")!=UMI_STATUS_ALREADY_EXISTS) return 2; if(!umi_ai_dev_context_assembler_contains(&c,"a")) return 3; if(umi_ai_dev_context_assembler_remove(&c,"a")!=UMI_STATUS_OK) return 4; return 0; }
