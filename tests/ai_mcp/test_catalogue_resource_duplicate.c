/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_catalogue_resource_duplicate.c
 *
 * PURPOSE:
 *   Verify MCP catalogue resource duplicate behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai/mcp/resource_catalogue.h"
int test_ai_mcp_catalogue_resource_duplicate(void){UmiAiMcpResourceCatalogue c;UmiAiMcpResourceDescriptor d={0};umi_ai_mcp_resource_catalogue_init(&c);strcpy(d.uri,"file:///a");assert(umi_ai_mcp_resource_catalogue_add(&c,&d)==UMI_STATUS_OK);assert(umi_ai_mcp_resource_catalogue_add(&c,&d)==UMI_STATUS_ALREADY_EXISTS);return 0;}
