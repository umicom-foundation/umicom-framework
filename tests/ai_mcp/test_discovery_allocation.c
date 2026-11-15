/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_discovery_allocation.c
 *
 * PURPOSE:
 *   Inject staged discovery allocation failures at individual and client boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdlib.h>

void UmiTestMcpDiscoveryFailAllocation(size_t fail_at);
static size_t discoveryAllocationNumber;
static size_t discoveryAllocationFailure;

void UmiTestMcpDiscoveryFailAllocation(size_t fail_at)
{
    discoveryAllocationNumber = 0U;
    discoveryAllocationFailure = fail_at;
}

static void *DiscoveryAllocate(size_t size)
{
    ++discoveryAllocationNumber;
    if (discoveryAllocationNumber == discoveryAllocationFailure) return NULL;
    return malloc(size);
}

/* Rename both the single-list and composed operations so a late allocation
 * refusal exercises the same transaction body without replacing linked APIs. */
#define malloc DiscoveryAllocate
#define umi_ai_mcp_discover_tools UmiTestMcpDiscoverTools
#define umi_ai_mcp_discover_resources UmiTestMcpDiscoverResources
#define umi_ai_mcp_discover_prompts UmiTestMcpDiscoverPrompts
#define umi_ai_mcp_client_discover UmiTestMcpClientDiscover
#define umi_ai_mcp_client_init UmiTestMcpUnusedClientInit
#define umi_ai_mcp_client_start UmiTestMcpUnusedClientStart
#define umi_ai_mcp_client_register_tools UmiTestMcpUnusedRegisterTools
#include "../../src/ai/mcp/discovery.c"
#include "../../src/ai/mcp/client.c"
#undef umi_ai_mcp_client_register_tools
#undef umi_ai_mcp_client_start
#undef umi_ai_mcp_client_init
#undef umi_ai_mcp_client_discover
#undef umi_ai_mcp_discover_prompts
#undef umi_ai_mcp_discover_resources
#undef umi_ai_mcp_discover_tools
#undef malloc
