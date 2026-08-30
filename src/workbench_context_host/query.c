/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/query.c
 *
 * PURPOSE:
 *   Initialise reusable context-host query requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/query.h"
#include <string.h>
void umi_workbench_context_host_query_init(UmiWorkbenchContextHostQuery *query,UmiWorkbenchContextHostQueryKind kind)
{if(query){memset(query,0,sizeof(*query));query->structure_size=(uint32_t)sizeof(*query);query->kind=kind;}}
