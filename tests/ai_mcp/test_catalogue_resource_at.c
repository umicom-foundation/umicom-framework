/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_catalogue_resource_at.c
 *
 * PURPOSE:
 *   Verify MCP catalogue resource at behavior.
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
int test_ai_mcp_catalogue_resource_at(void){UmiAiMcpResourceCatalogue c;UmiAiMcpResourceDescriptor d={0};umi_ai_mcp_resource_catalogue_init(&c);strcpy(d.uri,"file:///a");assert(umi_ai_mcp_resource_catalogue_add(&c,&d)==UMI_STATUS_OK);assert(umi_ai_mcp_resource_catalogue_at(&c,0)!=NULL);assert(umi_ai_mcp_resource_catalogue_at(&c,1)==NULL);return 0;}
