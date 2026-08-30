/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_catalogue_resource_add.c
 *
 * PURPOSE:
 *   Verify MCP catalogue resource add behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai/mcp/resource_catalogue.h"
int test_ai_mcp_catalogue_resource_add(void){UmiAiMcpResourceCatalogue c;UmiAiMcpResourceDescriptor d={0};umi_ai_mcp_resource_catalogue_init(&c);strcpy(d.uri,"file:///a");assert(umi_ai_mcp_resource_catalogue_add(&c,&d)==UMI_STATUS_OK);assert(c.count==1U);return 0;}
