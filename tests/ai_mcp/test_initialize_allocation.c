/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_initialize_allocation.c
 *
 * PURPOSE:
 *   Inject shared JSON allocation refusal into initialization publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/language_runtime/json_tree.h"
#include "../../src/ai/mcp/json_internal.h"

/* The fault seam now rejects the shared MCP object parser so decoder allocation regressions remain effective after parser ownership moves to json.c. The former implementation is retained for owner review. */
#if 0
static UmiStatus RejectInitializeTree(const void *bytes, size_t length,
    const UmiJsonTreeLimits *limits, const UmiCancellationToken *cancel, UmiJsonTree **out)
{
    (void)bytes; (void)length; (void)limits; (void)cancel;
    *out = NULL;
    return UMI_STATUS_OUT_OF_MEMORY;
}

#endif
static UmiStatus RejectInitializeObject(const char *json, UmiJsonTree **out)
{
    (void)json; (void)out;
    return UMI_STATUS_OUT_OF_MEMORY;
}

/* The fault variant retains the production decoder body under distinct names.
 * All ordinary cases link the actual public Framework implementation. */
/* Inject at the shared parser boundary instead of a JSON-tree call no longer made by codec.c. The former implementation is retained for owner review. */
#if 0
#define UmiJsonTreeCreate RejectInitializeTree
#endif
#define umi_ai_mcp_json_object_parse RejectInitializeObject
#define umi_ai_mcp_decode_initialize_result UmiTestMcpInitializeAllocationFailure
#define umi_ai_mcp_decode_tool UmiTestMcpUnusedDecodeTool
#define umi_ai_mcp_decode_resource UmiTestMcpUnusedDecodeResource
#define umi_ai_mcp_decode_prompt UmiTestMcpUnusedDecodePrompt
#define umi_ai_mcp_encode_initialize_params UmiTestMcpUnusedEncodeInitialize
#define umi_ai_mcp_encode_tool_call_params UmiTestMcpUnusedEncodeTool
#define umi_ai_mcp_encode_resource_read_params UmiTestMcpUnusedEncodeResource
#define umi_ai_mcp_encode_prompt_get_params UmiTestMcpUnusedEncodePrompt
#include "../../src/ai/mcp/codec.c"
#undef umi_ai_mcp_encode_prompt_get_params
#undef umi_ai_mcp_encode_resource_read_params
#undef umi_ai_mcp_encode_tool_call_params
#undef umi_ai_mcp_encode_initialize_params
#undef umi_ai_mcp_decode_prompt
#undef umi_ai_mcp_decode_resource
#undef umi_ai_mcp_decode_tool
#undef umi_ai_mcp_decode_initialize_result
#undef UmiJsonTreeCreate
#undef umi_ai_mcp_json_object_parse
