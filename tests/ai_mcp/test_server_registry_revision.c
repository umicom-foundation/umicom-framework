/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_server_registry_revision.c
 *
 * PURPOSE:
 *   Verify MCP server registry revision behavior.
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
 * Exercise test ai mcp server registry revision and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_server_registry_revision(void){UmiAiMcpServerRegistry r;UmiAiMcpServerDescriptor s;uint64_t rev;umi_ai_mcp_server_registry_init(&r);rev=r.revision;assert(umi_test_mcp_server(&s)==UMI_STATUS_OK);assert(umi_ai_mcp_server_registry_add(&r,&s)==UMI_STATUS_OK);assert(r.revision>rev);return 0;}
