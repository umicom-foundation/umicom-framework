/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_bridge_allocation.c
 *
 * PURPOSE:
 *   Inject registration allocation refusal without replacing the public SDK implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/tool_bridge.h"
#include <stdlib.h>

/* Compile the same transaction body under a fixture-only symbol. Other cases
 * call the public linked function, so this seam does not replace normal ABI
 * coverage or require a product allocator override. */
UmiStatus UmiTestMcpRegistrationAllocationFailure(UmiAiMcpToolBridge *bridge,
    UmiAiMcpSession *session, const UmiAiMcpToolCatalogue *catalogue,
    UmiAiToolRegistry *registry);

static void *RejectRegistrationAllocation(size_t size)
{
    (void)size;
    return NULL;
}

#define malloc RejectRegistrationAllocation
#define umi_ai_mcp_tool_bridge_register UmiTestMcpRegistrationAllocationFailure
#include "../../src/ai/mcp/tool_bridge.c"
#undef umi_ai_mcp_tool_bridge_register
#undef malloc
