/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_session_request_allocation.c
 *
 * PURPOSE:
 *   Inject ready-session result staging allocation refusal before transport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/mcp/session.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

UmiStatus UmiTestMcpSessionRequestAllocationFailure(UmiAiMcpSession *, const char *, const char *, char *, size_t);

static void *RejectSessionRequestAllocation(size_t size) { (void)size; return NULL; }
/* Keep the production session body while refusing only its result candidate.
 * Normal transport admission remains in the linked Framework implementation. */
#define malloc RejectSessionRequestAllocation
#define umi_ai_mcp_session_init UmiTestMcpRequestUnusedInit
#define umi_ai_mcp_session_initialize UmiTestMcpRequestUnusedInitialize
#define umi_ai_mcp_session_ping UmiTestMcpRequestUnusedPing
#define umi_ai_mcp_session_disconnect UmiTestMcpRequestUnusedDisconnect
#define UmiAiMcpSessionDisconnectChecked UmiTestMcpRequestUnusedDisconnectChecked
#define UmiAiMcpSessionRequestChecked UmiTestMcpSessionRequestAllocationFailure
#include "../../src/ai/mcp/session.c"
#undef UmiAiMcpSessionRequestChecked
#undef UmiAiMcpSessionDisconnectChecked
#undef umi_ai_mcp_session_disconnect
#undef umi_ai_mcp_session_ping
#undef umi_ai_mcp_session_initialize
#undef umi_ai_mcp_session_init
#undef malloc
