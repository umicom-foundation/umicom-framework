/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_list_allocation.c
 *
 * PURPOSE:
 *   Inject parser and callback-buffer allocation refusal into list visitation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/mcp/json_array.h"
#include "../../src/ai/mcp/json_internal.h"
#include <stdlib.h>

static int fault_mode;
static unsigned buffer_allocations;
static UmiStatus ParseForList(const char *json, UmiJsonTree **out)
{
    if (fault_mode == 1) return UMI_STATUS_OUT_OF_MEMORY;
    return umi_ai_mcp_json_object_parse(json, out);
}
static void *AllocateForList(size_t size)
{
    ++buffer_allocations;
    return fault_mode == 2 ? NULL : malloc(size);
}
/* Compile the production visitor under a private name. Only the parsing and
 * callback-buffer allocation boundaries differ; normal cases use the library. */
#define umi_ai_mcp_json_object_parse ParseForList
#define malloc AllocateForList
#define umi_ai_mcp_json_array_visit_objects UmiTestMcpListVisitImpl
#include "../../src/ai/mcp/json_array.c"
#undef umi_ai_mcp_json_array_visit_objects
#undef malloc
#undef umi_ai_mcp_json_object_parse

UmiStatus UmiTestMcpListVisitFault(int mode, const char *json, const char *key,
    UmiAiMcpJsonObjectVisitor visitor, void *data, size_t *count, unsigned *allocations)
{
    fault_mode = mode; buffer_allocations = 0U;
    UmiStatus status = UmiTestMcpListVisitImpl(json, key, visitor, data, count);
    *allocations = buffer_allocations; fault_mode = 0;
    return status;
}
