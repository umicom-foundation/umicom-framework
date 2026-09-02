/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_catalogue_resource_find.c
 *
 * PURPOSE:
 *   Verify MCP catalogue resource find behavior.
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
/*
 * Find test ai mcp catalogue resource while leaving the underlying catalogue or model
 * owned by this module.
 */
int test_ai_mcp_catalogue_resource_find(void){UmiAiMcpResourceCatalogue c;UmiAiMcpResourceDescriptor d={0};umi_ai_mcp_resource_catalogue_init(&c);strcpy(d.uri,"file:///a");assert(umi_ai_mcp_resource_catalogue_add(&c,&d)==UMI_STATUS_OK);assert(umi_ai_mcp_resource_catalogue_find(&c,"file:///a")!=NULL);assert(umi_ai_mcp_resource_catalogue_find(&c,"missing")==NULL);return 0;}
