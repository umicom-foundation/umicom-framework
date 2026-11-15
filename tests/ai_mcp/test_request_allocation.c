/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_request_allocation.c
 *
 * PURPOSE:
 *   Inject candidate-allocation refusal into MCP escaping and parameter publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/mcp/codec.h"
#include "umicom/protocol/json_writer.h"
#include "../../src/ai/mcp/json_internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void *RejectEncodingAllocation(size_t size) { (void)size; return NULL; }
/* Private production-body variants fail candidate allocation. Shared parser
 * allocation remains real here; a separate parser seam covers that boundary. */
#define malloc RejectEncodingAllocation
#define umi_ai_mcp_json_has_key UmiTestMcpEncodingUnusedHasKey
#define umi_ai_mcp_json_string UmiTestMcpEncodingUnusedString
#define umi_ai_mcp_json_raw UmiTestMcpEncodingUnusedRaw
#define umi_ai_mcp_json_escape_string UmiTestMcpEscapeAllocationFailure
#define umi_ai_mcp_json_object_parse UmiTestMcpEncodingUnusedParse
#include "../../src/ai/mcp/json.c"
#undef umi_ai_mcp_json_object_parse
#undef umi_ai_mcp_json_escape_string
#undef umi_ai_mcp_json_raw
#undef umi_ai_mcp_json_string
#undef umi_ai_mcp_json_has_key

#define umi_ai_mcp_decode_initialize_result UmiTestMcpEncodingUnusedDecodeInitialize
#define umi_ai_mcp_decode_tool UmiTestMcpEncodingUnusedDecodeTool
#define umi_ai_mcp_decode_resource UmiTestMcpEncodingUnusedDecodeResource
#define umi_ai_mcp_decode_prompt UmiTestMcpEncodingUnusedDecodePrompt
#define umi_ai_mcp_encode_initialize_params UmiTestMcpEncodeInitializeAllocationFailure
#define umi_ai_mcp_encode_tool_call_params UmiTestMcpEncodeToolAllocationFailure
#define umi_ai_mcp_encode_resource_read_params UmiTestMcpEncodeResourceAllocationFailure
#define umi_ai_mcp_encode_prompt_get_params UmiTestMcpEncodePromptAllocationFailure
#include "../../src/ai/mcp/codec.c"
#undef umi_ai_mcp_encode_prompt_get_params
#undef umi_ai_mcp_encode_resource_read_params
#undef umi_ai_mcp_encode_tool_call_params
#undef umi_ai_mcp_encode_initialize_params
#undef umi_ai_mcp_decode_prompt
#undef umi_ai_mcp_decode_resource
#undef umi_ai_mcp_decode_tool
#undef umi_ai_mcp_decode_initialize_result
#undef malloc
