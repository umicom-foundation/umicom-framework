/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_transport_allocation.c
 *
 * PURPOSE:
 *   Inject staging allocation refusal into the checked transport implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdlib.h>

static void *RejectTransportAllocation(size_t size)
{
    (void)size;
    return NULL;
}

/* Compile only the fault variant under distinct symbols. Normal cases link
 * the public Framework API. The included header declares these fixture names. */
#define malloc RejectTransportAllocation
#define umi_ai_mcp_transport_request UmiTestMcpUnusedDirectRequest
#define umi_ai_mcp_transport_notify UmiTestMcpUnusedDirectNotify
#define UmiAiMcpTransportRequestChecked UmiTestMcpTransportAllocationFailure
#include "../../src/ai/mcp/transport.c"
#undef UmiAiMcpTransportRequestChecked
#undef umi_ai_mcp_transport_notify
#undef umi_ai_mcp_transport_request
#undef malloc
