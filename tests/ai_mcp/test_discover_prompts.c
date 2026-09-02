/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_discover_prompts.c
 *
 * PURPOSE:
 *   Verify MCP discover prompts behavior through the Framework transport and existing AI runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "test_support.h"
/*
 * Exercise test ai mcp discover prompts and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_discover_prompts(void){UmiAiMcpSession s;UmiTestMcpTransportState state;UmiAiMcpPromptCatalogue c;assert(umi_test_mcp_ready_session(&s,&state)==UMI_STATUS_OK);assert(umi_ai_mcp_discover_prompts(&s,&c)==UMI_STATUS_OK);assert(c.count==2U);assert(umi_ai_mcp_prompt_catalogue_find(&c,"review")!=NULL);return 0;}
